#pragma once
#include <extfat.h>
char *map_entire_file(fileInfo *disk);
/*
this reads the contents of a file into a char * (entire file is in a huge block of memory)
disk is the struct that will contain information about the file


*/

void copy_file(fileInfo *disk, fileInfo *disk_copy);
/*
this copies file 1 into file 2
disk corresponds to a mapped image, disk_copy does not (yet)

*/
