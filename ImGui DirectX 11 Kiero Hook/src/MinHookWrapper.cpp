#include "../include/MinHookWrapper.h"

namespace MinHookWrapper {
    bool Initialize() {
        MH_STATUS status = MH_Initialize();
        if (status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
            return false;
        }
        return true;
    }

    bool Uninitialize() {
        MH_STATUS status = MH_Uninitialize();
        if (status != MH_OK) {
            return false;
        }
        return true;
    }

    bool EnableHook(LPVOID target) {
        MH_STATUS status = MH_EnableHook(target);
        if (status != MH_OK) {
            return false;
        }
        return true;
    }

    bool DisableHook(LPVOID target) {
        MH_STATUS status = MH_DisableHook(target);
        if (status != MH_OK) {
            return false;
        }
        return true;
    }

    bool EnableAllHooks() {
        MH_STATUS status = MH_EnableHook(MH_ALL_HOOKS);
        if (status != MH_OK) {
            return false;
        }
        return true;
    }

    bool DisableAllHooks() {
        MH_STATUS status = MH_DisableHook(MH_ALL_HOOKS);
        if (status != MH_OK) {
            return false;
        }
        return true;
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
        (void)status;
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
