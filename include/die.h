#ifndef DIELIB_H
#define DIELIB_H

#include <windows.h>

// flags
#define DIE_DEEPSCAN            0x00000001
#define DIE_HEURISTICSCAN       0x00000002
#define DIE_ALLTYPESSCAN        0x00000004
#define DIE_RECURSIVESCAN       0x00000008
#define DIE_VERBOSE             0x00000010
#define DIE_RESULTASXML         0x00010000
#define DIE_RESULTASJSON        0x00020000
#define DIE_RESULTASTSV         0x00040000
#define DIE_RESULTASCSV         0x00080000

#ifdef __cplusplus
extern "C" {
#endif

char *DIE_ScanFileA(char *pszFileName, unsigned int nFlags, char *pszDatabase);
wchar_t *DIE_ScanFileW(wchar_t *pwszFileName, unsigned int nFlags, wchar_t *pwszDatabase);
void DIE_FreeMemoryA(char *pszString);
void DIE_FreeMemoryW(char *pwszString);

#ifdef UNICODE
#define DIE_ScanFile DIE_ScanFileW
#define DIE_FreeMemory DIE_FreeMemoryW
#else
#define DIE_ScanFile DIE_ScanFileA
#define DIE_FreeMemory DIE_FreeMemoryA
#endif

#ifdef __cplusplus
}
#endif

#endif // DIELIB_H
