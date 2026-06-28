#include "../include/MinHookWrapper.h"
#include <iostream>

namespace MinHookWrapper {
    bool ExecuteMHOperation(MH_STATUS status, const char* operationName) {
        if (status != MH_OK) {
            std::cerr << "[MinHook] Failed to " << operationName << ": " << GetErrorString(status) << std::endl;
            return false;
        }
        return true;
    }

    bool Initialize() {
        MH_STATUS status = MH_Initialize();
        if (status == MH_ERROR_ALREADY_INITIALIZED) {
            return true;
        }
        return ExecuteMHOperation(status, "initialize");
    }

    bool Uninitialize() {
        return ExecuteMHOperation(MH_Uninitialize(), "uninitialize");
    }

    bool EnableHook(LPVOID target) {
        return ExecuteMHOperation(MH_EnableHook(target), "enable hook");
    }

    bool DisableHook(LPVOID target) {
        return ExecuteMHOperation(MH_DisableHook(target), "disable hook");
    }

    bool EnableAllHooks() {
        return ExecuteMHOperation(MH_EnableHook(MH_ALL_HOOKS), "enable all hooks");
    }

    bool DisableAllHooks() {
        return ExecuteMHOperation(MH_DisableHook(MH_ALL_HOOKS), "disable all hooks");
    }

    std::string GetErrorString(MH_STATUS status) {
        switch (status) {
            case MH_OK: return "OK";
            case MH_ERROR_ALREADY_INITIALIZED: return "Already initialized";
            case MH_ERROR_NOT_INITIALIZED: return "Not initialized";
            case MH_ERROR_ALREADY_CREATED: return "Already created";
            case MH_ERROR_NOT_CREATED: return "Not created";
            case MH_ERROR_ENABLED: return "Enabled";
            case MH_ERROR_DISABLED: return "Disabled";
            case MH_ERROR_NOT_EXECUTABLE: return "Not executable";
            case MH_ERROR_UNSUPPORTED_FUNCTION: return "Unsupported function";
            case MH_ERROR_MEMORY_ALLOC: return "Memory alloc failed";
            case MH_ERROR_MEMORY_PROTECT: return "Memory protect failed";
            case MH_ERROR_MODULE_NOT_FOUND: return "Module not found";
            case MH_ERROR_FUNCTION_NOT_FOUND: return "Function not found";
            default: return "Unknown error";
        }
    }

    Hook::Hook(LPVOID target, LPVOID detour)
        : m_target(target), m_detour(detour), m_enabled(false) {
        MH_STATUS status = MH_CreateHook(target, detour, nullptr);
        if (status != MH_OK) {
            std::cerr << "[MinHook] Failed to create hook: " << GetErrorString(status) << std::endl;
        }
    }

    Hook::~Hook() {
        if (m_enabled) {
            Disable();
        }
        MH_RemoveHook(m_target);
    }

    bool Hook::Enable() {
        if (EnableHook(m_target)) {
            m_enabled = true;
            return true;
        }
        return false;
    }

    bool Hook::Disable() {
        if (DisableHook(m_target)) {
            m_enabled = false;
            return true;
        }
        return false;
    }

    bool Hook::IsEnabled() const {
        return m_enabled;
    }
}
