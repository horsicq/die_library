#ifndef DIELIB_H
#define DIELIB_H

#include <windows.h>

// flags
#define DIE_DEEPSCAN                0x00000002
#define DIE_RESULTASXML             0x00000004
#define DIE_RESULTASJSON            0x00000008

#ifdef __cplusplus
extern "C" {
#endif

int  DIE_CreateScanHandle();
void DIE_CloseScanHandle(int nHandle);
char *DIE_ScanFileA(int nHandle,char *pszFileName,unsigned int nFlags);
wchar_t *DIE_ScanFileW(int nHandle,wchar_t *pwszFileName,unsigned int nFlags);

#ifdef UNICODE
#define DIE_ScanFile DIE_ScanFileW
#else
#define DIE_ScanFile DIE_ScanFileA
#endif

#ifdef __cplusplus
}
#endif

#endif // DIELIB_H