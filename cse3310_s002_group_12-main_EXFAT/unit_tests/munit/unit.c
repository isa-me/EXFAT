/* Example file for using µnit.
 *
 * µnit is MIT-licensed, but for this file and this file alone:
 *
 * To the extent possible under law, the author(s) of this file have
 * waived all copyright and related or neighboring rights to this
 * work.  See <https://creativecommons.org/publicdomain/zero/1.0/> for
 * details.
 *********************************************************************/

#include "munit.h"
#include <stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <extfat.h>
#include <getopt.h>
#include <assert.h>
//#include "/workspaces/cse3310_s002_group_12/include/printDirectory.h"

//#include "/workspaces/cse3310_s002_group_12/include/routines.h"
//#include "/workspaces/cse3310_s002_group_12/include/fileMapping.h"
#include "routines.h"

UInt32 BootChecksum
(
    UCHAR  * Sectors,        // points to an in-memory copy of the 11 sectors
    USHORT   BytesPerSector
)
{
    UInt32 NumberOfBytes = (UInt32)BytesPerSector * 11;
    UInt32 Checksum = 0;
    UInt32 Index;

    for (Index = 0; Index < NumberOfBytes; Index++)
    {
        if ((Index == 106) || (Index == 107) || (Index == 112))
        {
            continue;
        }
        Checksum = ((Checksum&1) ? 0x80000000 : 0) + (Checksum>>1) + (UInt32)Sectors[Index];
    }

    return Checksum;
}

/*
  This function is from /workspaces/cse3310_s002_group_12/src
  I was having problems with the h file in /workspaces/cse3310_s002_group_12/include/fileMapping.h
  So i decided to delcare the function here.
*/
char *map_entire_file(fileInfo *disk) {
     struct stat sb;//this will contain file size

      disk->fd = open(disk->fileName, O_RDONLY);//getting a file descriptor with the open() function 

    if (disk->fd == -1) {
        perror("Error opening input file");
         exit(1);
    }
    if (fstat(disk->fd, &sb) == -1) {//fstat reads values into sb, including file size 
        perror("fstat failed");
        return NULL;
    }
    disk->FileLength = sb.st_size; 
    char *file_data = mmap(NULL, //reading our file contents into a big block of memory with mmap
                            sb.st_size, 
                            PROT_READ, 
                            MAP_PRIVATE, 
                            disk->fd, 
                            0);

    if(file_data == MAP_FAILED)
    {
        perror("mmap failed");
    }

    return file_data;
}
void copy_file(fileInfo *disk, fileInfo *disk_copy) {
   //----------------------------------------------------------------
    //assert(disk != NULL && "Null pointer passed to copy file");

    // Creating the output file
    disk_copy->fd = open(disk_copy->fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (disk_copy->fd == -1) {
        perror("Unable to create output file");
        return;
    }

    // Increase the size of the output file to match the input file
    if (lseek(disk_copy->fd, disk->FileLength - 1, SEEK_SET) == -1) {
        perror("FAILed, unable to read output file size");
        return;
    }

    if (write(disk_copy->fd, "", 1) == -1) {
        perror("Error writing to output file");
        return;
    }

    // Map the output file to memory
    disk_copy->Data = mmap(0, 
                        disk->FileLength, 
                        PROT_READ | PROT_WRITE, 
                        MAP_SHARED, 
                        disk_copy->fd, 0);
    //assert((disk_copy->Data != MAP_FAILED) && "Unable to map outfile to memory" );

    disk_copy->FileLength = disk->FileLength;
    disk_copy->SectorSize = disk->SectorSize; 
    // Copy the data from the input file to the output file
    memcpy(disk_copy->Data, disk->Data, disk->FileLength);
}
void verify(Main_Boot* MB, Main_Boot* BB)
{
    //BytesPerSectorShift is an exponent in the file where 2^BytesPerSectorShift is the number of bytes per sector shift
    //very misleading name
    uint32_t bytesPerSector = 2 << (MB->BytesPerSectorShift-1);
    
    //assert(MB != NULL && "Memory allocation failed for Main_Boot");
    //assert(BB != NULL && "Memory allocation failed for Backup_Boot");

    UInt32 MBSumCalculated = BootChecksum((void *) MB, bytesPerSector);
    UInt32 BBSumCalculated = BootChecksum((void *) BB, bytesPerSector);

    UInt32 MBSumStored = *(UInt32 *)((void *)MB + 11*bytesPerSector);
    UInt32 BBSumStored = *(UInt32 *)((void *)BB + 11*bytesPerSector);
    munit_assert_uint32(MBSumCalculated, ==, BBSumCalculated);
    munit_assert_uint32(MBSumCalculated, ==, MBSumStored);
    munit_assert_uint32(BBSumCalculated, ==, BBSumStored);
    munit_assert_uint32(MBSumStored, ==, BBSumStored);

    /*if(MBSumCalculated != BBSumCalculated)
    {
        perror("Error: Main and Backup boot sectors not identical\n");

    }
    else if(MBSumCalculated != MBSumStored)
    {
        perror("Error: Main Boot stored checksum does not match calculated checksum");
    }
    else if(BBSumCalculated != BBSumStored)
    {
        perror("Error: Backup Boot stored checksum does not match calculated checksum");
    }
    else if(MBSumStored != BBSumStored)
    {
        perror("Error: Stored checksums of Main and Backup boot records do not match");
    }
    else
    {
        printf("Main and Backup boot sectors are the same\n");
    }*/
}
int cluster(Main_Boot * MB, int n)
{
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


static MunitResult test_map_entire_file(const MunitParameter params[], void* data){
  (void) params;
  (void) data;

  fileInfo disk = {0};
  disk.fileName = (char*)"test.image";
  disk.Data = map_entire_file(&disk);

  struct stat sb;
  int fd = open("test.image", O_RDONLY);
  fstat(fd, &sb);
  char *file_data = mmap(NULL, //reading our file contents into a big block of memory with mmap
                          sb.st_size, 
                          PROT_READ, 
                          MAP_PRIVATE, 
                          fd, 
                          0);

  munit_assert_int(disk.fd, !=, -1);
  munit_assert_int(disk.FileLength,==,sb.st_size);
  munit_assert_memory_equal(disk.FileLength, disk.Data, file_data);
  close(disk.fd);

  /*
  munit_assert_int(fd, !=, -1);
  close(fd);
  munmap(file_data, sb.st_size);*/

  munmap(disk.Data, disk.FileLength);
  

  return MUNIT_OK;
}

static MunitResult test_copy_file(const MunitParameter params[], void* data){
  (void) params;
  (void) data;

  fileInfo disk = {0};
  disk.fileName = (char*)"test.image";
  disk.Data = map_entire_file(&disk);

  fileInfo disk_copy = {0};
  disk_copy.fileName = (char*)"copy.image";
  copy_file(&disk, &disk_copy); 
  disk_copy.Data = map_entire_file(&disk);   

  munit_assert_memory_equal(disk.FileLength, disk.Data, disk_copy.Data);
  munit_assert_ptr_not_equal(disk.Data, disk_copy.Data);
  munit_assert_int(disk.FileLength,==,disk_copy.FileLength);
  munit_assert_int(disk.SectorSize,==,disk_copy.SectorSize);

  close(disk.fd);
  close(disk_copy.fd);

  munmap(disk.Data, disk.FileLength);
  munmap(disk_copy.Data, disk_copy.FileLength);

  return MUNIT_OK;
}

static MunitResult test_verify(const MunitParameter params[], void* data){
  (void) params;
  (void) data;

  fileInfo disk = {0};
  disk.fileName = (char*)"test.image";
  disk.Data = map_entire_file(&disk);

    disk.M_Boot = disk.Data;
    disk.SectorSize = 2 << (disk.M_Boot->BytesPerSectorShift-1);
    disk.B_Boot = (void*) disk.M_Boot + 12 * disk.SectorSize;
  verify(disk.M_Boot,disk.B_Boot);

  close(disk.fd);

  munmap(disk.Data, disk.FileLength);

  return MUNIT_OK;
}

static MunitResult test_cluster(const MunitParameter params[], void* data){
  (void) params;
  (void) data;
  int r = 0;
  int n = 0;
  fileInfo disk = {0};
  disk.fileName = (char*)"test.image";
  disk.Data = map_entire_file(&disk);


  disk.M_Boot = disk.Data;
  disk.SectorSize = 2 << (disk.M_Boot->BytesPerSectorShift-1);
  disk.B_Boot = (void*) disk.M_Boot + 12 * disk.SectorSize;
  
  // n should be >=0
  // upper limit for n is size of image / cluster size
  // r should be <=dataLength - clustersize.
  // cluster size is bytes per sector * sector per clustor
  // dataLength is < fileLength - cluster size

  uint32_t bytesPerSector = 2 << ((*(disk.M_Boot)).BytesPerSectorShift - 1);
  uint32_t sectorsPerCluster = 2 << ((*(disk.M_Boot)).SectorsPerClusterShift - 1);
  if(sectorsPerCluster == 0)
  {
    sectorsPerCluster = 1;
  }
  uint32_t cluster_size = bytesPerSector * sectorsPerCluster;
  int dataLength = disk.FileLength;
  //-------------------Domain partitioning -------------------
  r = cluster(disk.M_Boot,n);
  munit_assert_int(r, <=, dataLength-(int)cluster_size);

  n=1;
  r = cluster(disk.M_Boot,n);
  munit_assert_int(r, <=, dataLength-(int)cluster_size);

  n=16;
  r = cluster(disk.M_Boot,n);
  munit_assert_int(r, <=, (int)dataLength-(int)cluster_size);

  n= disk.M_Boot->ClusterCount;
  r = cluster(disk.M_Boot,n);
  munit_assert_int(r, <=, dataLength-(int)cluster_size);
  //----------------------------------------------------------

  close(disk.fd);

  munmap(disk.Data, disk.FileLength);

  return MUNIT_OK;
}

/* an attempt was made to use parameters in the test for processArgs
static MunitParameterEnum test_params[] = {
  { (char*) "-i",NULL },
  { (char*) "-c",NULL},
  { (char*) "-v", NULL },
  { (char*) "test.image", NULL },
  { NULL, NULL },
};*/
static MunitTest test_suite_tests[] = {
  { (char*) "/example/map", test_map_entire_file, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { (char*) "/example/copy", test_copy_file, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { (char*) "/example/verify", test_verify, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { (char*) "/example/cluster", test_cluster, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  //{ (char*) "/example/processCommandInput", test_processCommandInput, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite test_suite = {
  (char*) "", test_suite_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

//#include "/workspaces/cse3310_s002_group_12/include/fileMapping.h"
#include <stdlib.h>

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
  return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}
