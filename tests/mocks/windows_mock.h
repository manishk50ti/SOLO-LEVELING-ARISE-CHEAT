#pragma once
// Mock Windows types for cross-platform testing

#include <cstdint>
#include <cstddef>

#ifndef _WIN32

typedef void* LPVOID;
typedef void* HMODULE;
typedef void* HANDLE;
typedef unsigned long DWORD;
typedef int BOOL;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// Mock MODULEINFO
typedef struct _MODULEINFO {
    LPVOID lpBaseOfDll;
    DWORD SizeOfImage;
    LPVOID EntryPoint;
} MODULEINFO;

// Mock Windows API functions
inline HMODULE GetModuleHandleA(const char*) { return nullptr; }
inline HANDLE GetCurrentProcess() { return nullptr; }
inline BOOL GetModuleInformation(HANDLE, HMODULE, MODULEINFO* info, DWORD) {
    if (info) {
        info->SizeOfImage = 0;
    }
    return FALSE;
}

#endif // _WIN32
