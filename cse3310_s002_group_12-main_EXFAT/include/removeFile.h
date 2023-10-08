#pragma once

#ifdef __cplusplus
extern "C"

#endif

#include<stdio.h>
#include<stdint.h>
#include<extfat.h>
#include<printDirectory.h>
#include<imageFileHandling.h>
#define UInt16 uint16_t
#define UCHAR unsigned char
#define WCHAR unsigned short





//will take a pointer to a mapped fileInfo struct and the name of the file
//returns a struct with helpful information like the offset of the file, what cluster it starts in, parsed filename (char **), etc

void targetFile(FileDirectoryEntry * F, fileInfo* disk, int nesting, char* inTargetFile);
int deleteFile(imageFile * imf);
int getTargetCluster(fileInfo* disk,int n, int nesting, char* inTargetFile);
int deleteEntry(fileInfo* disk,StreamExtensionEntry* StreamEx,FILE* imageFile);
int targetCluster(Main_Boot * MB, int n);
