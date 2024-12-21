#ifndef DIELIB_H
#define DIELIB_H

#ifdef _WIN32
#include <windows.h>
#endif

// flags
#define DIE_DEEPSCAN 0x00000001
#define DIE_HEURISTICSCAN 0x00000002
#define DIE_ALLTYPESSCAN 0x00000004
#define DIE_RECURSIVESCAN 0x00000008
#define DIE_VERBOSE 0x00000010
#define DIE_RESULTASXML 0x00010000
#define DIE_RESULTASJSON 0x00020000
#define DIE_RESULTASTSV 0x00040000
#define DIE_RESULTASCSV 0x00080000

#ifdef __cplusplus
extern "C" {
#endif

char *DIE_ScanFileA(char *pszFileName, unsigned int nFlags, char *pszDatabase);
wchar_t *DIE_ScanFileW(wchar_t *pwszFileName, unsigned int nFlags, wchar_t *pwszDatabase);
char *DIE_ScanMemoryA(char *pMemory, int nMemorySize, unsigned int nFlags, char *pszDatabase);
wchar_t *DIE_ScanMemoryW(char *pMemory, int nMemorySize, unsigned int nFlags, wchar_t *pwszDatabase);
int DIE_LoadDatabaseA(char *pszDatabase);
int DIE_LoadDatabaseW(wchar_t *pwszDatabase);
char *DIE_ScanFileExA(char *pszFileName, unsigned int nFlags);
wchar_t *DIE_ScanFileExW(wchar_t *pwszFileName, unsigned int nFlags);
char *DIE_ScanMemoryExA(char *pMemory, int nMemorySize, unsigned int nFlags);
wchar_t *DIE_ScanMemoryExW(char *pMemory, int nMemorySize, unsigned int nFlags);
void DIE_FreeMemoryA(char *pszString);
void DIE_FreeMemoryW(wchar_t *pwszString);

#ifdef _WIN32
int DIE_VB_ScanFile(wchar_t *pwszFileName, unsigned int nFlags, wchar_t *pwszDatabase, wchar_t *pwszBuffer, int nBufferSize);
#endif

#ifdef UNICODE
#define DIE_ScanFile DIE_ScanFileW
#define DIE_ScanMemory DIE_ScanMemoryW
#define DIE_LoadDatabase DIE_LoadDatabaseW
#define DIE_ScanFileEx DIE_ScanFileExW
#define DIE_ScanMemoryEx DIE_ScanMemoryExW
#define DIE_FreeMemory DIE_FreeMemoryW
#else
#define DIE_ScanFile DIE_ScanFileA
#define DIE_ScanMemory DIE_ScanMemoryA
#define DIE_LoadDatabase DIE_LoadDatabaseA
#define DIE_ScanFileEx DIE_ScanFileExA
#define DIE_ScanMemoryEx DIE_ScanMemoryExA
#define DIE_FreeMemory DIE_FreeMemoryA
#endif

#ifdef __cplusplus
}
#endif

#endif  // DIELIB_H
