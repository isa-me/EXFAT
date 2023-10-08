#pragma once


typedef struct
{
    char *fileIn;
    char *fileOut;
    int hasDel;
    int hasC;
    int hasV;
    int hasD;
    int hasX;
    char *targetFile;
} inputParamaters;
//struct contains crucial data from command line arguments read with processCommandInput

inputParamaters processCommandInput(int argc, char *argv[]);
//returns a struct with data from command line arguments
//takes argc and argv from main
