#include <stdio.h>
#include <getopt.h>
#include <processArgs.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

//this code being seperated will likely be usefull when merging, so ill keep it here!



/*
----------------------------------------------------------
storedInput holds the paramaters input by a
user through the command line
----------------------------------------------------------
char *fileIn: String with name of input file (-i filenamehere)
char *fileOut: String with name of output file (-o filenamehere)
int hasV: Verify was selected (-v)
int hasC: Copy was selected (-c)
int hasD: Print Directories option was selected (-d)
*/


/*
----------------------------------------------------------
function that fills struct named "storedInput" containing
Strings and ints (ints are only used as booleans):
----------------------------------------------------------
Help (-h)
-char *fileIn: String with name of input file (-i filenamehere)
-char *fileOut: String with name of output file (-o filenamehere)
-int *hasV: Verify option selected (-v)
-int *hasC: Copy option selected(-c)
*/
inputParamaters processCommandInput(int argc, char *argv[])
{
    assert(argc >= 1 && "argc provided is 0 or lower");
    assert(argv != NULL && "argv provided is null");
    inputParamaters storedInput = {0};
    

    const char fileExtension[6] = {'.', 'i','m','a','g','e'};//we use this to check if our file is a .image
    int opts;
    while((opts = getopt(argc, argv, "i:hcvdo:D:x:")) != -1)
    {
        switch(opts)
        {
            case 'h':
                printf("here are the commands extfat can be run with!\n");
                printf("extfat can be run by typing ./extfat in the bash console\n");
                printf("-h this help menu :D\n");
                printf("-i \"input\" input being a filename\n");
                printf("-o \"output\" output being a filename and is optional (if not specified, the output file is the same as the input file)\n");
                printf("-c copies the \"input\" file to the \"output\" file.\n");
                printf("-v verifies the main and backup boot sectors of the \"input\" file are the same.\n");
                printf("-d prints the files and directories of the provided ExFAT image\n");
                printf("-x \"target\" extracts \"target\" file from input file into output file (will overwrite output file)\n");
                printf("-D \"target file\" deletes the target file from the input image\n");
                exit(0);//band aid solution, keeps all the other logic and error handling from running
                break;
            case 'c':
                printf("recognized the \"-c\" option\n");
                storedInput.hasC=1;
                if(storedInput.hasX)
                {
                    perror("Error: -x and -c options incompatible");
                    exit(1);
                }
                break;
            case 'd':
                printf("recognized the \"-d\" option\n");
                storedInput.hasD=1;
                break;
            case 'v':
                printf("recognized the \"-v\" option\n");
                storedInput.hasV = 1;
                break;
            case 'i':
                printf("recognized the \"-i\" option\n");
                storedInput.fileIn=optarg;
                printf("file input: %s\n", storedInput.fileIn);
                assert(storedInput.fileIn[strlen(storedInput.fileIn) - 1] != '\0' && "Input file was not null-terminated");
                break;
            case 'o':
                printf("recognized the \"-o\" option\n");
                storedInput.fileOut=optarg;
                printf("file output: %s\n", storedInput.fileOut);
                if (storedInput.fileOut == NULL) {
                    perror("Error: output file name is NULL");
                break;
            case 'x':
                if(storedInput.hasDel)
                {
                    perror("Error: -D and -x options incompatible");
                    exit(1);
                }
                if(storedInput.hasC == 1)
                {
                    perror("Error: -x and -c options incompatible");
                    exit(1);
                }


                storedInput.hasX = 1;
                printf("recognized the \"-x\" option\n");
                storedInput.targetFile=optarg;
                printf("targeted file: %s\n", storedInput.targetFile);
                if(storedInput.targetFile == NULL)
                {
                    perror("Error: targeted file name is NULL");
                    exit(1);
                }


                break;
            case 'D':
                printf("recognized the \"-D\" option\n");
                if(storedInput.hasX)
                {
                    perror("Error: -D and -x options incompatible");
                    exit(1);
                }

                storedInput.targetFile=optarg;
                storedInput.hasDel=1;
                printf("file target: %s\n", storedInput.targetFile);
                if (storedInput.targetFile == NULL) {
                    perror("Error: targetFile file name is NULL");
                    exit(1);
                }
                break;
            }

        }
    }



    //making sure we have an input file
    if(storedInput.fileIn == 0)
    {
        perror("Specify an input file with the -i option\n");
        exit(1);
    }




    //checking to see if the input file is .image
    for(int i = 0; i < 6; i++)
    {
        if(storedInput.fileIn[i + strlen(storedInput.fileIn) - 6] != fileExtension[i])
        {
            //assert(i + strlen(storedInput.fileIn) - 6 >= 0u && "Out of Range");
            perror("\nEnter a .image file for input\n");
            exit(1);
        }
    }

    return storedInput;
}