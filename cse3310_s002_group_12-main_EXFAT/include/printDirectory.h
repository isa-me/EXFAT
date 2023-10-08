#pragma once
#include <stdint.h>
#include<imageFileHandling.h>

//all directory entry structs were moved to imageFileHandling.h


/*
------------------------------
vvvvv entryType (byte 0) vvvvv

> typeCode(5 bits) defines the type of a given directory
> TypeImportance 0 means critical, 1 means benign
> TypeCategory 0 means primary, 1 means secondary
> isInUse 0 means directory is not used, 1 means it is in use


vvvvv customDefined (byte 1 to byte 19) vvvvv

***while mandatory, it does not store information we need to print directories (that I know of)
it may come up later, wouldnt surprise me :b


vvvvv firstCluster (byte 20 to byte 23) vvvvv

contains the index of the first cluster of an allocation in the Cluster Heap


vvvvv dataLength (byte 24 to byte 31) vvvvv

DataLength field describes the size, in bytes, of the data the associated cluster allocation contains
Max Value = ClusterCount * 2^(SectorsPerClusterShift) * 2^(BytesPerSectorShift)


------------------------------

*/

//prints all directorires starting from root
void printAllDirectories(fileInfo * disk);


//prints the selected entry
void printDirectoryEntry(FileDirectoryEntry * F, fileInfo* disk, int nesting, uint32_t currentCluster);

/*
prints directories associated with the image file. the first cluster is calculated using the following:
MB->ClusterHeapOffset * bytesPerSector + (bytesPerSector * sectorsPerCluster * (MB->FirstClusterOfRootDirectory-2));
*/
int cluster(Main_Boot * MB, int n);

/*
prints directories associated with the image file.
using "directoryEntries" (a generic directory) as an array of entries to go through
each entry and print it
*/
int getSelectedCluster(fileInfo * disk, int n, int nesting);


         
