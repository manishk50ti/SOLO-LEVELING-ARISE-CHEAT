#pragma once
// Mock MinHook types for cross-platform testing

#include "windows_mock.h"

// MinHook status codes
typedef enum {
    MH_OK = 0,
    MH_ERROR_ALREADY_INITIALIZED,
    MH_ERROR_NOT_INITIALIZED,
    MH_ERROR_ALREADY_CREATED,
    MH_ERROR_NOT_CREATED,
    MH_ERROR_ENABLED,
    MH_ERROR_DISABLED,
    MH_ERROR_NOT_EXECUTABLE,
    MH_ERROR_UNSUPPORTED_FUNCTION,
    MH_ERROR_MEMORY_ALLOC,
    MH_ERROR_MEMORY_PROTECT,
    MH_ERROR_MODULE_NOT_FOUND,
    MH_ERROR_FUNCTION_NOT_FOUND
} MH_STATUS;

#define MH_ALL_HOOKS nullptr

// Mock MinHook functions - configurable return values for testing
namespace MockMinHook {
    inline MH_STATUS g_initializeResult = MH_OK;
    inline MH_STATUS g_uninitializeResult = MH_OK;
    inline MH_STATUS g_createHookResult = MH_OK;
    inline MH_STATUS g_enableHookResult = MH_OK;
    inline MH_STATUS g_disableHookResult = MH_OK;
    inline MH_STATUS g_removeHookResult = MH_OK;
    inline int g_initializeCallCount = 0;
    inline int g_uninitializeCallCount = 0;
    inline int g_createHookCallCount = 0;
    inline int g_enableHookCallCount = 0;
    inline int g_disableHookCallCount = 0;
    inline int g_removeHookCallCount = 0;

    inline void Reset() {
        g_initializeResult = MH_OK;
        g_uninitializeResult = MH_OK;
        g_createHookResult = MH_OK;
        g_enableHookResult = MH_OK;
        g_disableHookResult = MH_OK;
        g_removeHookResult = MH_OK;
        g_initializeCallCount = 0;
        g_uninitializeCallCount = 0;
        g_createHookCallCount = 0;
        g_enableHookCallCount = 0;
        g_disableHookCallCount = 0;
        g_removeHookCallCount = 0;
    }
}

inline MH_STATUS MH_Initialize() {
    MockMinHook::g_initializeCallCount++;
    return MockMinHook::g_initializeResult;
}

inline MH_STATUS MH_Uninitialize() {
    MockMinHook::g_uninitializeCallCount++;
    return MockMinHook::g_uninitializeResult;
}

inline MH_STATUS MH_CreateHook(LPVOID target, LPVOID detour, LPVOID* original) {
    MockMinHook::g_createHookCallCount++;
    (void)target; (void)detour; (void)original;
    return MockMinHook::g_createHookResult;
}

inline MH_STATUS MH_EnableHook(LPVOID target) {
    MockMinHook::g_enableHookCallCount++;
    (void)target;
    return MockMinHook::g_enableHookResult;
}

inline MH_STATUS MH_DisableHook(LPVOID target) {
    MockMinHook::g_disableHookCallCount++;
    (void)target;
    return MockMinHook::g_disableHookResult;
}

inline MH_STATUS MH_RemoveHook(LPVOID target) {
    MockMinHook::g_removeHookCallCount++;
    (void)target;
    return MockMinHook::g_removeHookResult;
}
