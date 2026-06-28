#pragma once
#include <windows.h>
#include <string>
#include <MinHook.h>
#include <functional>

namespace MinHookWrapper {
    // Initialize MinHook
    bool Initialize();

    // Uninitialize MinHook
    bool Uninitialize();

    // Create a hook
    template<typename T>
    bool CreateHook(LPVOID target, LPVOID detour, T* original) {
        MH_STATUS status = MH_CreateHook(target, detour, reinterpret_cast<LPVOID*>(original));
        return status == MH_OK;
    }

    // Execute a MinHook operation with error logging
    bool ExecuteMHOperation(MH_STATUS status, const char* operationName);

    // Enable a hook
    bool EnableHook(LPVOID target);

    // Disable a hook
    bool DisableHook(LPVOID target);

    // Enable all hooks
    bool EnableAllHooks();

    // Disable all hooks
    bool DisableAllHooks();

    // Get error string
    std::string GetErrorString(MH_STATUS status);

    // Hook class for RAII management
    class Hook {
    public:
        Hook(LPVOID target, LPVOID detour);
        ~Hook();

        bool Enable();
        bool Disable();
        bool IsEnabled() const;

    private:
        LPVOID m_target;
        LPVOID m_detour;
        bool m_enabled;
    };
}
