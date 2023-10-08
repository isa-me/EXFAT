#pragma once
#include<extfat.h>
void verify(Main_Boot* MB, Main_Boot* BB);
//this compares the boot sectors in the file inFile (that's the file's name)
//usually these are the main and backup boot sectors