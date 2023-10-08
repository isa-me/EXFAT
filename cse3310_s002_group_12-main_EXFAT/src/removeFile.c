#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <extfat.h>
#include <printDirectory.h>
#include <stdlib.h>
#include <stdint.h>
#include <removeFile.h>
#include <string.h>
#include <stdio.h>
#include <imageFileHandling.h>


//deletes a file based on user input
int deleteFile(imageFile * imf)
{  
   uint32_t currentCluster = imf->FirstCluster;
   uint32_t * FAT = imf->disk->FAT;
   uint32_t bytesPerSector = 2 << (imf->disk->M_Boot->BytesPerSectorShift - 1);
   uint32_t sectorsPerCluster = 2 << (imf->disk->M_Boot->SectorsPerClusterShift - 1);
   uint32_t zeroInt = 0;//we use this when writing to FAT

   if(sectorsPerCluster == 0)
   {
      sectorsPerCluster = 1;
   }


   uint64_t clusterSize = bytesPerSector * sectorsPerCluster;
   uint32_t clustersToDelete = (imf->dataLength / clusterSize) + (imf->dataLength % clusterSize ==0 ? 0 : 1);//basically divide and ceil
   void * emptyCluster = calloc(clusterSize,sizeof(char));
   FILE * image = fopen(imf->disk->fileName,"r+");
   if(image == NULL)
   {
      perror("Couldn't open file");
      exit(1);
   }
   uint64_t clusterOffset = 0;
   uint64_t FATOffset = bytesPerSector * imf->disk->M_Boot->FatOffset;
   while(currentCluster > 0 && clustersToDelete > 0)
   {
      //writing zero bytes to the clusters our file is in
      clusterOffset = cluster(imf->disk->M_Boot,currentCluster);
      fseek(image,clusterOffset,SEEK_SET);
      fwrite(emptyCluster,1,clusterSize,image);

      //writing zero bytes to the file's entries in the File Allocation Table
      fseek(image,FATOffset + currentCluster*sizeof(uint32_t),SEEK_SET);
      fwrite(&zeroInt,1,sizeof(uint32_t),image);
      currentCluster = FAT[currentCluster];
      clustersToDelete--;
   }
   free(emptyCluster);
   //file: eliminated (probably)



   //now we get the entry type and flip the inUse bit
   fseek(image, imf->fileDirectoryOffset,SEEK_SET);
   uint8_t flippedEntry =  *(uint8_t *)(imf->disk->Data + imf->fileDirectoryOffset);
   flippedEntry = flippedEntry & (0b01111111);


   fwrite((void *)&flippedEntry,1,1,image);

   fclose(image);


   return 0;
}



/*

int targetCluster(Main_Boot * MB, int n)
{
   uint32_t bytesPerSector = 2 << (MB->BytesPerSectorShift - 1);
   uint32_t sectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1);
   

   if(n==0)
      n = ((MB->ClusterHeapOffset * bytesPerSector) + ((MB->FirstClusterOfRootDirectory - 2) * bytesPerSector * sectorsPerCluster));
   else
      n = ((MB->ClusterHeapOffset * bytesPerSector) + ((n - 2) * bytesPerSector * sectorsPerCluster));
   
   return n;
}

//name of the file and the address of an int containing the number of characters left to print
void targetFileName(char * name, uint8_t * nameLen, char * fullFileName)
{
   for(int i = 0; i < 30 && *nameLen != 0; i+=2)
   {
      strcat(fullFileName,&name[i]);
      *nameLen -=1;
   }
}

int getTargetCluster(fileInfo* disk,int n, int nesting, char* inTargetFile)
{  
   n = cluster(disk->M_Boot,n);
   GenericDirectoryStruct *x = n + disk->Data;

   
   int i = 0;
   while (x[i].EntryType){i++;}
   i--;
   int num_entries = i;
   i=0;

   FileDirectoryEntry * F;

   while (i<num_entries)
   {
      if(x[i].EntryType == 0x85)
      {
         F=(FileDirectoryEntry *) x+i;
         targetFile(F,disk,nesting, inTargetFile);
      }
      i++;
   }

   return i+1;
}


void targetFile(FileDirectoryEntry * F, fileInfo* disk, int nesting, char* inTargetFile)
{  
   char temp = *inTargetFile;
   char selectedFileName[1024] = "\0";
   //immediately following every FileDirectoryEntry is 1 streamextension and at least 1 filename extension
   StreamExtensionEntry * StreamEx = (StreamExtensionEntry *) F+1;
   FilenameExtensionEntry * Fname =(FilenameExtensionEntry *) F+2;
   uint8_t numChars = StreamEx->NameLength;//we need this so we know when to stop printing

   while(numChars > 0)
   {
      targetFileName(Fname->FileName,&numChars,selectedFileName);
      Fname++;
   }

   if((uint8_t)F->FileAttributes[0] & 0b00010001)//we are just testing the bit that shows whether it is a directory
   {
      nesting++;
      //going to the cluster specified in the stream extension entry and repeating this process with 1 more indent
      if(strcmp(inTargetFile, selectedFileName) == 0 || !temp)
         {
            inTargetFile = "\0";
            getTargetCluster(disk,StreamEx->FirstCluster,nesting, inTargetFile);
         }
      return;
   }
   if(strcmp(inTargetFile, selectedFileName) == 0 || !temp)
   {
      FILE* imageFile = fopen("test.image", "r+");
      deleteEntry(disk,StreamEx, imageFile);
      fclose(imageFile);
      //F->EntryType -= 1<<7;
   }
}
int deleteEntry(fileInfo* disk,StreamExtensionEntry* StreamEx, FILE* imageFile)
{
   int targetLocation = targetCluster(disk->M_Boot,StreamEx->FirstCluster);
   fseek(imageFile,targetLocation,SEEK_SET);
   for(int x=0; x<StreamEx->DataLength; x++)
   {
      fwrite("\0",1,1,imageFile);  
   }
   return 0;
}

*/