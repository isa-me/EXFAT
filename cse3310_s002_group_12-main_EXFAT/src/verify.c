/* #include<extfat.h> */
#include<stdio.h>
#include<string.h>
#include<fileMapping.h>
#include<stdlib.h>
#include<routines.h>
#include<assert.h>

/*
verify the main and backup boot sectors
this will make sure the contents of the Main/Backup boot
match eachother
*/
void verify(Main_Boot* MB, Main_Boot* BB)
{
    //BytesPerSectorShift is an exponent in the file where 2^BytesPerSectorShift is the number of bytes per sector shift
    //very misleading name
    uint32_t bytesPerSector = 2 << (MB->BytesPerSectorShift-1);
    
    assert(MB != NULL && "Memory allocation failed for Main_Boot");
    assert(BB != NULL && "Memory allocation failed for Backup_Boot");

    UInt32 MBSumCalculated = BootChecksum((void *) MB, bytesPerSector);
    UInt32 BBSumCalculated = BootChecksum((void *) BB, bytesPerSector);

    UInt32 MBSumStored = *(UInt32 *)((void *)MB + 11*bytesPerSector);//checksums are stored at the start of the 12th sector
    UInt32 BBSumStored = *(UInt32 *)((void *)BB + 11*bytesPerSector);
    /*
    printf("%d Stored %d calculated\n", MBSumStored, MBSumCalculated);
    printf("%d Stored %d calculated\n", BBSumStored, BBSumCalculated);
    */
    if(MBSumCalculated != BBSumCalculated)
    {
        perror("Error: Main and Backup boot sectors not identical\n");
        exit(1);
    }
    else if(MBSumCalculated != MBSumStored)
    {
        perror("Error: Main Boot stored checksum does not match calculated checksum");
        exit(1);
    }
    else if(BBSumCalculated != BBSumStored)
    {
        perror("Error: Backup Boot stored checksum does not match calculated checksum");
        exit(1);
    }
    else if(MBSumStored != BBSumStored)
    {
        perror("Error: Stored checksums of Main and Backup boot records do not match");
        exit(1);
    }
    else
    {
        printf("Main and Backup boot sectors are the same\n");
    }

}