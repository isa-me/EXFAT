#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <extfat.h>
#include <processArgs.h>
#include <fileMapping.h>
#include <printDirectory.h>
#include <stdint.h>
#include<imageFileHandling.h>


//name of the file and the address of an int containing the number of characters left to print
void printFileName(char * name, uint8_t * nameLen)
{
    for(int i = 0; i < 30 && *nameLen > 0; i+=2)
    {
      printf("%c", name[i]);
      *nameLen -=1;
        
    }
}


//this function exists because it makes more sense in main than getSelectedCluster(&disk,0,0)
void printAllDirectories(fileInfo * disk)
{
   getSelectedCluster(disk,0,0);
}


int cluster(Main_Boot * MB, int n)
{
   if(n < 0)
   {
      perror("Attempted to access negative cluster\n");
      exit(1);
   }
   uint32_t bytesPerSector = 2 << (MB->BytesPerSectorShift - 1);
   uint32_t sectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1);
   
   //band aid solution
   if(sectorsPerCluster == 0)
   {
      sectorsPerCluster = 1;
   }

   if(n==0)
      n = ((MB->ClusterHeapOffset * bytesPerSector) + ((MB->FirstClusterOfRootDirectory - 2) * bytesPerSector * sectorsPerCluster));
   else
      n = ((MB->ClusterHeapOffset * bytesPerSector) + ((n - 2) * bytesPerSector * sectorsPerCluster));
   
   return n;
}
//n is the number cluster we are selecting, nesting is how nested that cluster is in the directory (0 for root)




int getSelectedCluster(fileInfo* disk,int n, int nesting)
{  
   uint32_t bytesPerSector = 2 << (disk->M_Boot->BytesPerSectorShift - 1);
   uint32_t sectorsPerCluster = 2 << (disk->M_Boot->SectorsPerClusterShift - 1);
   if(sectorsPerCluster == 0)
   {
      sectorsPerCluster = 1;
   }

   int clusterSize = bytesPerSector*sectorsPerCluster;

   int clusterOffset = cluster(disk->M_Boot,n);
   


   GenericDirectoryStruct *x = clusterOffset + disk->Data;
   uint32_t maxEntriesPerCluster = clusterSize/sizeof(DirectoryEntry);

   uint32_t * FAT = disk->FAT;

   uint32_t i = 0;


   FileDirectoryEntry * F;

   while (x[i].EntryType)
   {
      //checking that we are in bounds for our cluster
      if(i >= maxEntriesPerCluster)
      {
         i = 0;
         n = FAT[n];
         if(n==-1)
         {
            perror("error in FAT table\n");
         }
         clusterOffset = cluster(disk->M_Boot, n);
         x = clusterOffset + disk->Data;
      }
      if(x[i].EntryType == 0x85)
      {
         F=(FileDirectoryEntry *) x+i;
         printDirectoryEntry(F,disk,nesting, n);
         printf("\n");
      }

      i++;
   }

   return i+1;
}
//DataLength


//takes a FileDirectoryEntry *, pointer to disk struct, and number indicating how far nested the file is in directory structure
void printDirectoryEntry(FileDirectoryEntry * F, fileInfo* disk, int nesting, uint32_t currentCluster)
{
   //immediately following every FileDirectoryEntry is 1 streamextension and at least 1 filename extension
   StreamExtensionEntry * StreamEx = (StreamExtensionEntry *) F+1;



   FilenameExtensionEntry * Fname =(FilenameExtensionEntry *) F+2;
   uint8_t numChars = StreamEx->NameLength;//we need this so we know when to stop printing

   uint32_t * FAT = disk->FAT;

   uint32_t bytesPerSector = 2 << (disk->M_Boot->BytesPerSectorShift - 1);
   uint32_t sectorsPerCluster = 2 << (disk->M_Boot->SectorsPerClusterShift - 1);
   if(sectorsPerCluster == 0)
   {
      sectorsPerCluster = 1;
   }

   uint64_t clusterSize = bytesPerSector*sectorsPerCluster;



   void * clusterBoundary = disk->Data + cluster(disk->M_Boot, currentCluster) + clusterSize;


   //checking that we are in bounds
   if((uint64_t)StreamEx >= (uint64_t)clusterBoundary)
   {
      currentCluster = FAT[currentCluster];
      StreamEx = disk->Data + cluster(disk->M_Boot,currentCluster);
      numChars = StreamEx->NameLength;
      Fname = (FilenameExtensionEntry *)StreamEx + 1;
      clusterBoundary = disk->Data + cluster(disk->M_Boot, currentCluster) + clusterSize;

      //printf("StreamEx cluster boundary encountered\n");
   }


   for(int i = 0; i < nesting; i++)//we use tabs to indicate how far nested in the directory structure a file is
   {
      printf("\t");
   }
   while(numChars > 0)
   {
      //checking that we are in clutser bounds
      if((uint64_t)Fname >= (uint64_t)clusterBoundary)
      {
         currentCluster = FAT[currentCluster];
         Fname = disk->Data + cluster(disk->M_Boot, currentCluster);
         clusterBoundary = disk->Data + cluster(disk->M_Boot, currentCluster) + clusterSize;
         //printf("FnameX cluster boundary encountered");
      }
      printFileName(Fname->FileName,&numChars);
      Fname++;

   }
//   printf("\toffset %u\n", cluster(disk->M_Boot, ((StreamExtensionEntry *) F + 1)->FirstCluster));//   use this when debugging
   if((uint8_t)F->FileAttributes[0] & 0b00010000)//we are just testing the bit that shows whether it is a directory
   {
      //if this runs a directory was detected
      printf(">>>\n");
      nesting++;
      //going to the cluster specified in the stream extension entry and repeating this process with 1 more indent
      getSelectedCluster(disk,StreamEx->FirstCluster,nesting);
   }





}