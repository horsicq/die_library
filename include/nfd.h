#ifndef NFDLIB_H
#define NFDLIB_H

#include <windows.h>

// flags
#define NFD_RECURSIVE               0x00000001
#define NFD_DEEPSCAN                0x00000002
#define NFD_RESULTASXML             0x00000004
#define NFD_RESULTASJSON            0x00000008
#define NFD_HEURISTICSCAN           0x00000010

#ifdef __cplusplus
extern "C" {
#endif

int  NFD_CreateScanHandle();
void NFD_CloseScanHandle(int nHandle);
char *NFD_ScanFileA(int nHandle,char *pszFileName,unsigned int nFlags);
wchar_t *NFD_ScanFileW(int nHandle,wchar_t *pwszFileName,unsigned int nFlags);

#ifdef UNICODE
#define NFD_ScanFile NFD_ScanFileW
#else
#define NFD_ScanFile NFD_ScanFileA
#endif

#ifdef __cplusplus
}
#endif

#endif // NFDLIB_H