#include <gtest/gtest.h>
#include "minhook_mock.h"
#include <string>

// Re-implement MinHookWrapper logic using mocked MinHook for testing
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

template<typename T>
bool CreateHook(LPVOID target, LPVOID detour, T* original) {
    MH_STATUS status = MH_CreateHook(target, detour, reinterpret_cast<LPVOID*>(original));
    return status == MH_OK;
}

class Hook {
public:
    Hook(LPVOID target, LPVOID detour)
        : m_target(target), m_detour(detour), m_enabled(false) {
        MH_CreateHook(target, detour, nullptr);
    }

    ~Hook() {
        if (m_enabled) {
            Disable();
        }
        MH_RemoveHook(m_target);
    }

    bool Enable() {
        if (EnableHook(m_target)) {
            m_enabled = true;
            return true;
        }
        return false;
    }

    bool Disable() {
        if (DisableHook(m_target)) {
            m_enabled = false;
            return true;
        }
        return false;
    }

    bool IsEnabled() const {
        return m_enabled;
    }

private:
    LPVOID m_target;
    LPVOID m_detour;
    bool m_enabled;
};

} // namespace MinHookWrapper

// --- Initialize Tests ---

class MinHookInitTest : public ::testing::Test {
protected:
    void SetUp() override {
        MockMinHook::Reset();
    }
};

TEST_F(MinHookInitTest, Initialize_Success) {
    MockMinHook::g_initializeResult = MH_OK;
    EXPECT_TRUE(MinHookWrapper::Initialize());
    EXPECT_EQ(MockMinHook::g_initializeCallCount, 1);
}

TEST_F(MinHookInitTest, Initialize_AlreadyInitialized_StillReturnsTrue) {
    MockMinHook::g_initializeResult = MH_ERROR_ALREADY_INITIALIZED;
    EXPECT_TRUE(MinHookWrapper::Initialize());
}

TEST_F(MinHookInitTest, Initialize_OtherError_ReturnsFalse) {
    MockMinHook::g_initializeResult = MH_ERROR_MEMORY_ALLOC;
    EXPECT_FALSE(MinHookWrapper::Initialize());
}

TEST_F(MinHookInitTest, Uninitialize_Success) {
    MockMinHook::g_uninitializeResult = MH_OK;
    EXPECT_TRUE(MinHookWrapper::Uninitialize());
    EXPECT_EQ(MockMinHook::g_uninitializeCallCount, 1);
}

TEST_F(MinHookInitTest, Uninitialize_Error_ReturnsFalse) {
    MockMinHook::g_uninitializeResult = MH_ERROR_NOT_INITIALIZED;
    EXPECT_FALSE(MinHookWrapper::Uninitialize());
}

// --- Hook Enable/Disable Tests ---

class MinHookEnableTest : public ::testing::Test {
protected:
    void SetUp() override {
        MockMinHook::Reset();
    }
};

TEST_F(MinHookEnableTest, EnableHook_Success) {
    MockMinHook::g_enableHookResult = MH_OK;
    int dummy = 0;
    EXPECT_TRUE(MinHookWrapper::EnableHook(&dummy));
    EXPECT_EQ(MockMinHook::g_enableHookCallCount, 1);
}

TEST_F(MinHookEnableTest, EnableHook_Error_ReturnsFalse) {
    MockMinHook::g_enableHookResult = MH_ERROR_NOT_CREATED;
    int dummy = 0;
    EXPECT_FALSE(MinHookWrapper::EnableHook(&dummy));
}

TEST_F(MinHookEnableTest, DisableHook_Success) {
    MockMinHook::g_disableHookResult = MH_OK;
    int dummy = 0;
    EXPECT_TRUE(MinHookWrapper::DisableHook(&dummy));
    EXPECT_EQ(MockMinHook::g_disableHookCallCount, 1);
}

TEST_F(MinHookEnableTest, DisableHook_Error_ReturnsFalse) {
    MockMinHook::g_disableHookResult = MH_ERROR_NOT_CREATED;
    int dummy = 0;
    EXPECT_FALSE(MinHookWrapper::DisableHook(&dummy));
}

TEST_F(MinHookEnableTest, EnableAllHooks_Success) {
    MockMinHook::g_enableHookResult = MH_OK;
    EXPECT_TRUE(MinHookWrapper::EnableAllHooks());
}

TEST_F(MinHookEnableTest, DisableAllHooks_Success) {
    MockMinHook::g_disableHookResult = MH_OK;
    EXPECT_TRUE(MinHookWrapper::DisableAllHooks());
}

// --- GetErrorString Tests ---

class GetErrorStringTest : public ::testing::Test {};

TEST_F(GetErrorStringTest, OK) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_OK), "OK");
}

TEST_F(GetErrorStringTest, AlreadyInitialized) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_ALREADY_INITIALIZED), "Already initialized");
}

TEST_F(GetErrorStringTest, NotInitialized) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_NOT_INITIALIZED), "Not initialized");
}

TEST_F(GetErrorStringTest, AlreadyCreated) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_ALREADY_CREATED), "Already created");
}

TEST_F(GetErrorStringTest, NotCreated) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_NOT_CREATED), "Not created");
}

TEST_F(GetErrorStringTest, Enabled) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_ENABLED), "Enabled");
}

TEST_F(GetErrorStringTest, Disabled) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_DISABLED), "Disabled");
}

TEST_F(GetErrorStringTest, NotExecutable) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_NOT_EXECUTABLE), "Not executable");
}

TEST_F(GetErrorStringTest, UnsupportedFunction) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_UNSUPPORTED_FUNCTION), "Unsupported function");
}

TEST_F(GetErrorStringTest, MemoryAlloc) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_MEMORY_ALLOC), "Memory alloc failed");
}

TEST_F(GetErrorStringTest, MemoryProtect) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_MEMORY_PROTECT), "Memory protect failed");
}

TEST_F(GetErrorStringTest, ModuleNotFound) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_MODULE_NOT_FOUND), "Module not found");
}

TEST_F(GetErrorStringTest, FunctionNotFound) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(MH_ERROR_FUNCTION_NOT_FOUND), "Function not found");
}

TEST_F(GetErrorStringTest, UnknownStatus) {
    EXPECT_EQ(MinHookWrapper::GetErrorString(static_cast<MH_STATUS>(999)), "Unknown error");
}

// --- Hook Class Tests ---

class HookClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        MockMinHook::Reset();
    }
};

TEST_F(HookClassTest, Constructor_CreatesHook) {
    int target = 0, detour = 0;
    {
        MinHookWrapper::Hook hook(&target, &detour);
        EXPECT_EQ(MockMinHook::g_createHookCallCount, 1);
        EXPECT_FALSE(hook.IsEnabled());
    }
}

TEST_F(HookClassTest, Enable_SetsState) {
    MockMinHook::g_enableHookResult = MH_OK;
    int target = 0, detour = 0;
    MinHookWrapper::Hook hook(&target, &detour);

    EXPECT_TRUE(hook.Enable());
    EXPECT_TRUE(hook.IsEnabled());
}

TEST_F(HookClassTest, Enable_Failure_DoesNotSetState) {
    MockMinHook::g_enableHookResult = MH_ERROR_NOT_CREATED;
    int target = 0, detour = 0;
    MinHookWrapper::Hook hook(&target, &detour);

    EXPECT_FALSE(hook.Enable());
    EXPECT_FALSE(hook.IsEnabled());
}

TEST_F(HookClassTest, Disable_ClearsState) {
    MockMinHook::g_enableHookResult = MH_OK;
    MockMinHook::g_disableHookResult = MH_OK;
    int target = 0, detour = 0;
    MinHookWrapper::Hook hook(&target, &detour);

    hook.Enable();
    EXPECT_TRUE(hook.IsEnabled());

    EXPECT_TRUE(hook.Disable());
    EXPECT_FALSE(hook.IsEnabled());
}

TEST_F(HookClassTest, Destructor_DisablesIfEnabled) {
    MockMinHook::g_enableHookResult = MH_OK;
    MockMinHook::g_disableHookResult = MH_OK;
    int target = 0, detour = 0;
    {
        MinHookWrapper::Hook hook(&target, &detour);
        hook.Enable();
    }
    // Destructor should have called Disable and RemoveHook
    EXPECT_EQ(MockMinHook::g_disableHookCallCount, 1);
    EXPECT_EQ(MockMinHook::g_removeHookCallCount, 1);
}

TEST_F(HookClassTest, Destructor_RemovesHookEvenIfNotEnabled) {
    int target = 0, detour = 0;
    {
        MinHookWrapper::Hook hook(&target, &detour);
        // Don't enable
    }
    EXPECT_EQ(MockMinHook::g_disableHookCallCount, 0);
    EXPECT_EQ(MockMinHook::g_removeHookCallCount, 1);
}

// --- CreateHook Template Tests ---

class CreateHookTemplateTest : public ::testing::Test {
protected:
    void SetUp() override {
        MockMinHook::Reset();
    }
};

TEST_F(CreateHookTemplateTest, CreateHook_Success) {
    MockMinHook::g_createHookResult = MH_OK;
    int target = 0, detour = 0;
    void* original = nullptr;
    EXPECT_TRUE(MinHookWrapper::CreateHook(&target, &detour, &original));
    EXPECT_EQ(MockMinHook::g_createHookCallCount, 1);
}

TEST_F(CreateHookTemplateTest, CreateHook_Failure) {
    MockMinHook::g_createHookResult = MH_ERROR_MEMORY_ALLOC;
    int target = 0, detour = 0;
    void* original = nullptr;
    EXPECT_FALSE(MinHookWrapper::CreateHook(&target, &detour, &original));
}
