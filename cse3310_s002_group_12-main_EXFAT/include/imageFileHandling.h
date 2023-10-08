#pragma once

#ifdef __cplusplus
extern "C"

#endif

#include<stdint.h>
#include<extfat.h>
#include<printDirectory.h>
#define UInt16 uint16_t
#define UCHAR unsigned char
#define WCHAR unsigned short


   typedef struct
   {
      union
      {
         uint8_t EntryType;
         struct
         {
            uint8_t TypeCode : 5;
            uint8_t TypeImportance : 1;
            uint8_t TypeCategory : 1;
            uint8_t InUse : 1;
         };
      };
      uint8_t CustomDefined[19];
      uint32_t FirstCluster;
      uint64_t DataLength;
   } GenericDirectoryStruct;

    //note the filename only takes up around 16 bytes
    //so the mentioned array size is NOT ACCURATE
    //I just dont have the time to code in the rest of the parameters
   struct secondaryEntry
         {
            union
            {
               uint8_t EntryType;
               struct
               {
                  uint8_t TypeCode : 5;
                  uint8_t TypeImportance : 1;
                  uint8_t TypeCategory : 1;
                  uint8_t InUse : 1;
               };
            };
            uint8_t GeneralSecondaryFlags : 8;
            char FileName[30];                  //this should be redefined to size 18 imo, but its 4am and I want to sleep :)
         };



//these always follow FileDirectoryEntries
typedef struct StreamExtensionEntry
{
    char EntryType;
    char GeneralSecondaryFlags;
    char Reserved;
    uint8_t NameLength;
    uint16_t NameHash;
    char Reserved2[2];
    uint64_t ValidDataLength;
    char Reserved3[4];
    uint32_t FirstCluster;
    uint64_t DataLength;





}StreamExtensionEntry;

//Each FileDirectoryEntry has 1 or more of these after the StreamExtensionEntry
typedef struct FilenameExtensionEntry
{
    uint8_t EntryType;
    char GeneralSecondaryFlags;
    char FileName[30];
} FilenameExtensionEntry;

typedef struct DirectoryEntry
{
    uint8_t EntryType;
    char contents[31];
} DirectoryEntry;

//Every file or directory has 1 of these
typedef struct FileDirectoryEntry
{
    char EntryType;
    char SecondaryCount;//number of secondary entries, min 2 (streamextension and filenames)
    uint16_t SetChecksum;
    char FileAttributes[2];//the 5th bit (bit 4) says whether it is a file (0) or directory(1), use bitwise AND
    char Reserved[2];
    char CreationTime[4];
    char LastModified[4];
    char LastAccessed[4];
    char Create10msIncrement;
    char LastModified10msIncrement;
    char LastAccessed10msIncrement;
    char Reserved2[9];





}FileDirectoryEntry;





//a file in an exFat image
typedef struct imageFile
{
    char ** parsed_fileName;
    //filename separated, parsed by / characters
    uint64_t fileOffset;
    uint64_t dataLength;
    uint64_t fileDirectoryOffset;
    uint8_t nesting;
    uint32_t FirstCluster;
    fileInfo * disk;

}imageFile;



//will take a pointer to a mapped fileInfo struct and the name of the file
//returns a struct with helpful information like the offset of the file, what cluster it starts in, parsed filename (char **), etc
imageFile findFileInImage(fileInfo * disk, char * fileName);

void * extractFile(imageFile * imf);



void freeImageFile(imageFile * imf);