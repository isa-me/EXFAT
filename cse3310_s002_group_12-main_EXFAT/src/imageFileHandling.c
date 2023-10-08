#include <extfat.h>
#include <printDirectory.h>
#include <stdlib.h>
#include <stdint.h>
#include <imageFileHandling.h>
#include <string.h>
#include <stdio.h>


//see imageFileHandling.h for simpler explanations






//frees the memory stored in struct
void freeImageFile(imageFile * imf)
{
    for(int i = 0; i < imf->nesting; i++)
    {
        free(imf->parsed_fileName[i]);
    }
    free(imf->parsed_fileName);
}
//trim newline
void trim(char * str)
{
    if(str[strlen(str) - 1] == '\n')
    {
        str[strlen(str) - 1] = 0;
    }
}
//finds the targeted file in an image, returns a struct containing file data
imageFile findFileInImage(fileInfo * disk, char * fileName)
{
   uint32_t bytesPerSector = 2 << (disk->M_Boot->BytesPerSectorShift - 1);
   uint32_t sectorsPerCluster = 2 << (disk->M_Boot->SectorsPerClusterShift - 1);
   
   //band aid solution
   if(sectorsPerCluster == 0)
   {
      sectorsPerCluster = 1;
   }
    uint64_t clusterSize = bytesPerSector*sectorsPerCluster;
    uint32_t * FAT = disk->FAT;
    uint32_t currentCluster;



    trim(fileName);//we do not want newlines
    imageFile imf = {0};
    imf.nesting = 1;
    for(int i = 0; fileName[i];i++)//looping until we hit null terminator in filename
    {
        if(fileName[i] == '/')//counting slashes to see how many directories we must traverse
        {
            imf.nesting++;
        }
    }
    imf.parsed_fileName = calloc(imf.nesting, sizeof(char *));//an array that can hold as many strings as we need





    uint8_t pos = 0;
    uint8_t token = 0;

    //this loop just reads the given filename into several file/directory names
    //strtok is weird and i dont like it
    for(int i = 0; i < (int)strlen(fileName)+1; i++)
    {
        if(fileName[i]== '/' || fileName[i]==0)
        {
            imf.parsed_fileName[token] = calloc(i + 1 - pos, sizeof(char));
            for(int j = 0; j < i - pos; j++)
            {

                imf.parsed_fileName[token][j] = fileName[pos + j];
            }
            pos=i+1;
            token++;
        }
    }


    //parse filename: done




    //now we go find the cluster offset of our file

    uint8_t mismatch = 0;//we will use this when checking filenames
    uint8_t match = 0;
    uint8_t confirmedMatch = 0;
    uint32_t clusterOffset = cluster(disk->M_Boot,0);//offset of root
    currentCluster = 2;//start at 0 since we're in root

    //if we reach or go over this we have to go to next entry in FAT
    void * clusterBoundary = disk->Data + clusterOffset + clusterSize;


    DirectoryEntry * D = 0;
    StreamExtensionEntry * StreamEx = 0;
    FilenameExtensionEntry * FN = 0;



    for(uint8_t i = 0; i < imf.nesting; i++)
    {


        char * nameSegment = imf.parsed_fileName[i];
        uint8_t nameLen = strlen(imf.parsed_fileName[i]);
        D = disk->Data + clusterOffset;


        while(D->EntryType)
        {

            if((uint64_t)D >= (uint64_t)clusterBoundary)
            {
                currentCluster = FAT[currentCluster];
                clusterOffset = cluster(disk->M_Boot, currentCluster);
                clusterBoundary = disk->Data + clusterOffset + clusterSize;
                D = disk->Data + clusterOffset;
            }
            if(D->EntryType == 0x85)
            {
                StreamEx = (StreamExtensionEntry *)  D + 1;

                if((uint64_t)StreamEx >= (uint64_t)clusterBoundary)
                {
        
                    currentCluster = FAT[currentCluster];
                    clusterOffset = cluster(disk->M_Boot, currentCluster);
                    clusterBoundary = disk->Data + clusterOffset + clusterSize;
                    StreamEx = disk->Data + clusterOffset;
                }


                if(StreamEx->NameLength == nameLen)//checking that number of characters in name is same
                {
                    int index = 0;
                    FN = (FilenameExtensionEntry *) StreamEx+1;
                    while(index < nameLen)
                    {
                        if((uint64_t)FN >= (uint64_t)clusterBoundary)
                        {
                            currentCluster = FAT[currentCluster];
                            clusterOffset = cluster(disk->M_Boot, currentCluster);
                            clusterBoundary = disk->Data + clusterOffset + clusterSize;
                            FN = disk->Data + clusterOffset;
                        }


                        for(int i = 0; i < 30; i+=2)
                        {
                            if(FN->FileName[i] != nameSegment[index])
                            {
                                //if characters don't match
                                mismatch = 1;

                                break;
                            }
                            else
                            {
                            //    printf("%c",nameSegment[index] ); //helpful when debugging
                                index++;
                            }
                            if(index == nameLen)
                            {
                                match = 1;
                                break;
                            }
                        }
                        if(mismatch)
                        {
                            mismatch = 0;
                            break;
                        }

                        FN = FN+1;//going to the next filename extension entry

                    }
                }
            }
            if(match)
            {
                break;
            }
            else
            {

                D = (void *)D+sizeof(DirectoryEntry);

            }
        }
        if(match)
        {
                if((int)currentCluster < 0)
                {
                    perror("File not allocated\n");
                    freeImageFile(&imf);
                    exit(1);
                }
            match=0;
            currentCluster = StreamEx->FirstCluster;
            clusterOffset = cluster(disk->M_Boot,currentCluster);
            clusterBoundary = disk->Data + clusterOffset + clusterSize;
            
            if(i+1 == imf.nesting)
            {
                confirmedMatch = 1;
            }
            //moving to the next cluster
        }





        
    }
//    printf("offset %u\n", clusterOffset);

    //reading a bunch of values into this struct so we can use them in various operations


    //NOTE: if the FirstCluster is 0 the file isn't allocated

    if(confirmedMatch == 1)
    {
        imf.fileOffset = clusterOffset;
        imf.FirstCluster = StreamEx->FirstCluster;
        imf.dataLength = StreamEx->DataLength;
        imf.fileDirectoryOffset = (void *) D - disk->Data;
        imf.disk = disk;
    }
    else
    {
        freeImageFile(&imf);
        perror("Error: File Not Found");
        exit(1);
    }

    return imf;
}


//takes a given imagefile and extracts the data into a big block of memory
void * extractFile(imageFile * imf)
{
    uint32_t bytesPerSector = 2 << (imf->disk->M_Boot->BytesPerSectorShift - 1);
    uint32_t sectorsPerCluster = 2 << (imf->disk->M_Boot->SectorsPerClusterShift - 1);
    //band aid solution
    if(sectorsPerCluster == 0)
    {
        sectorsPerCluster = 1;
    }
    uint32_t clusterSize = bytesPerSector*sectorsPerCluster;
    uint32_t clustersToCopy = (imf->dataLength / clusterSize) + (imf->dataLength % clusterSize ==0 ? 0 : 1);//basically divide and ceil
    uint32_t clustersCopied = 0;




    int * FAT= imf->disk->FAT;
    int numClusters = 0;
    int currentCluster = imf->FirstCluster;
    void * fileInMemory = calloc(imf->dataLength, sizeof(char));
//    printf("datalen %ld\n", imf->dataLength);

    void * clusterInMemory = 0;



    //use this loop when going through clusters
    while(currentCluster > 0 && clustersToCopy > 0)
    {

        //getting cluster offset with cluster function
        clusterInMemory = imf->disk->Data + cluster(imf->disk->M_Boot, currentCluster);
        if(clustersToCopy == 1)//if we are on last cluster
        {
            //copy the number of bytes remaining
            memcpy(fileInMemory + clusterSize*clustersCopied, clusterInMemory, imf->dataLength%clusterSize);
        }
        else
        {
            //copying the whole cluster
            memcpy(fileInMemory + clusterSize*clustersCopied, clusterInMemory, clusterSize);
        }

        numClusters ++;
        currentCluster = FAT[currentCluster];
        clustersCopied++;
        clustersToCopy--;


    }


    //return the pointer to the huge file we just mapped
    return fileInMemory;
}