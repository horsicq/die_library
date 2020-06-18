#include <stdio.h>
#include "nfd.h"

int main(int argc, char* argv[])
{
    char *pszFileName="C:\\Windows\\notepad.exe";
    int nHandle=0;
    char *pszResult=0;
    
    nHandle=NFD_CreateScanHandle(); 
    
    pszResult=NFD_ScanFileA(nHandle,pszFileName,NFD_RECURSIVE+NFD_DEEPSCAN);
    printf("%s",pszResult);
    
    NFD_CloseScanHandle(nHandle);

    return 0;
}

