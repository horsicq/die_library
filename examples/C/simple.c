#include <stdio.h>
#include "die.h"

int main(int argc, char* argv[])
{
    char *pszFileName="C:\\Windows\\notepad.exe";
    //char *pszDatabase = "C:\\db";
    char *pszDatabase = "$data/db";
    int nHandle=0;
    char *pszResult=0;
    
    pszResult=DIE_ScanFileA(pszFileName, DIE_DEEPSCAN | DIE_HEURISTICSCAN | DIE_RECURSIVESCAN, pszDatabase);
    printf("%s",pszResult);
    
    DIE_FreeMemoryA(pszResult);

    return 0;
}

