#include "../include/GameHooks.h"
#include "../include/MinHookWrapper.h"
#include "../include/PatternScanner.h"
#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

namespace GameHooks {
    Features g_features;
    bool g_initialized = false;

    // Original function pointers
    namespace Damage {
        GetIncreaseValueFunc o_GetIncreaseValue_ShieldConversion = nullptr;
        GetIncreaseValueFunc o_GetIncreaseValue_ShieldRemain = nullptr;
        GetAddDamageFunc o_GetAddDamage = nullptr;
        GetRateFunc o_GetRate = nullptr;
    }

    namespace Stats {
        AddStatFunc o_AddStat = nullptr;
        GetStatValueFunc o_GetStatValue = nullptr;
        SetStatFunc o_SetStat = nullptr;
    }

    namespace Hunter {
        SetHunterHPFunc o_SetHunterHP = nullptr;
        SetHunterActiveFunc o_SetHunterActive = nullptr;
    }

    namespace Cooldown {
        OnSkillCoolTimeReducedFunc o_OnSkillCoolTimeReduced = nullptr;
        ResetCoolTimeFunc o_ResetCoolTime = nullptr;
    }

    namespace Speed {
        SetSpeedFunc o_SetSpeed = nullptr;
    }

    namespace AntiCheat {
        LogPlayerTakeDamageFunc o_LogPlayerTakeDamage = nullptr;
        AntiCheatCommonPlayerTakeDamageFunc o_AntiCheatCommonPlayerTakeDamage = nullptr;
    }

    bool Initialize() {
        HMODULE gameAssembly = GetModuleHandleA("GameAssembly.dll");
        if (!gameAssembly) {
            std::cerr << "[GameHooks] Failed to get GameAssembly.dll module" << std::endl;
            return false;
        }

        // Delay for 1 minute
        std::this_thread::sleep_for(std::chrono::minutes(1));

        Offsets::GameAssemblyBase = reinterpret_cast<uintptr_t>(gameAssembly);
        std::cout << "[GameHooks] GameAssembly.dll base: 0x" << std::hex << Offsets::GameAssemblyBase << std::endl;

        // Initialize MinHook
        if (!MinHookWrapper::Initialize()) {
            std::cerr << "[GameHooks] Failed to initialize MinHook" << std::endl;
            return false;
        }

        // Hook damage functions
        uintptr_t getIncValueAddr = Offsets::GetAbsoluteAddress(Offsets::Damage::GetIncreaseValue_ShieldConversion);
        if (MinHookWrapper::CreateHook(
            reinterpret_cast<LPVOID>(getIncValueAddr),
            reinterpret_cast<LPVOID>(&Damage::hk_GetIncreaseValue_ShieldConversion),
            &Damage::o_GetIncreaseValue_ShieldConversion
        )) {
            MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(getIncValueAddr));
            std::cout << "[GameHooks] Hooked GetIncreaseValue_ShieldConversion at 0x" << std::hex << getIncValueAddr << std::endl;
        }

        uintptr_t getAddDamageAddr = Offsets::GetAbsoluteAddress(Offsets::Damage::GetAddDamage_ElementGSSkill);
        if (MinHookWrapper::CreateHook(
            reinterpret_cast<LPVOID>(getAddDamageAddr),
            reinterpret_cast<LPVOID>(&Damage::hk_GetAddDamage),
            &Damage::o_GetAddDamage
        )) {
            MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(getAddDamageAddr));
            std::cout << "[GameHooks] Hooked GetAddDamage at 0x" << std::hex << getAddDamageAddr << std::endl;
        }

        uintptr_t getRateAddr = Offsets::GetAbsoluteAddress(Offsets::Damage::GetRate_ElementGSSkillCritical);
        if (MinHookWrapper::CreateHook(
            reinterpret_cast<LPVOID>(getRateAddr),
            reinterpret_cast<LPVOID>(&Damage::hk_GetRate),
            &Damage::o_GetRate
        )) {
            MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(getRateAddr));
            std::cout << "[GameHooks] Hooked GetRate at 0x" << std::hex << getRateAddr << std::endl;
        }

        // Hook stat functions
        uintptr_t addStatAddr = Offsets::GetAbsoluteAddress(Offsets::Stats::AddStat);
        if (MinHookWrapper::CreateHook(
            reinterpret_cast<LPVOID>(addStatAddr),
            reinterpret_cast<LPVOID>(&Stats::hk_AddStat),
            &Stats::o_AddStat
        )) {
            MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(addStatAddr));
            std::cout << "[GameHooks] Hooked AddStat at 0x" << std::hex << addStatAddr << std::endl;
        }

        uintptr_t getStatValueAddr = Offsets::GetAbsoluteAddress(Offsets::Stats::GetStatValue);
        if (MinHookWrapper::CreateHook(
            reinterpret_cast<LPVOID>(getStatValueAddr),
            reinterpret_cast<LPVOID>(&Stats::hk_GetStatValue),
            &Stats::o_GetStatValue
        )) {
            MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(getStatValueAddr));
            std::cout << "[GameHooks] Hooked GetStatValue at 0x" << std::hex << getStatValueAddr << std::endl;
        }

        // Hook hunter functions
        uintptr_t setHunterHPAddr = Offsets::GetAbsoluteAddress(Offsets::Hunter::SetHunterHP);
        if (MinHookWrapper::CreateHook(
            reinterpret_cast<LPVOID>(setHunterHPAddr),
            reinterpret_cast<LPVOID>(&Hunter::hk_SetHunterHP),
            &Hunter::o_SetHunterHP
        )) {
            MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(setHunterHPAddr));
            std::cout << "[GameHooks] Hooked SetHunterHP at 0x" << std::hex << setHunterHPAddr << std::endl;
        }

        // Hook cooldown functions
        uintptr_t onCoolTimeReducedAddr = Offsets::GetAbsoluteAddress(Offsets::Skills::OnSkillCoolTimeReduced);
        if (MinHookWrapper::CreateHook(
            reinterpret_cast<LPVOID>(onCoolTimeReducedAddr),
            reinterpret_cast<LPVOID>(&Cooldown::hk_OnSkillCoolTimeReduced),
            &Cooldown::o_OnSkillCoolTimeReduced
        )) {
            MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(onCoolTimeReducedAddr));
            std::cout << "[GameHooks] Hooked OnSkillCoolTimeReduced at 0x" << std::hex << onCoolTimeReducedAddr << std::endl;
        }

        // Hook speed functions
        uintptr_t setSpeedAddr = Offsets::GetAbsoluteAddress(Offsets::Speed::SetSpeed);
        if (MinHookWrapper::CreateHook(
            reinterpret_cast<LPVOID>(setSpeedAddr),
            reinterpret_cast<LPVOID>(&Speed::hk_SetSpeed),
            &Speed::o_SetSpeed
        )) {
            MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(setSpeedAddr));
            std::cout << "[GameHooks] Hooked SetSpeed at 0x" << std::hex << setSpeedAddr << std::endl;
        }

        // Hook anti-cheat functions
        uintptr_t logPlayerTakeDamageAddr = Offsets::GetAbsoluteAddress(Offsets::AntiCheat::LogPlayerTakeDamage);
        if (MinHookWrapper::CreateHook(
            reinterpret_cast<LPVOID>(logPlayerTakeDamageAddr),
            reinterpret_cast<LPVOID>(&AntiCheat::hk_LogPlayerTakeDamage),
            &AntiCheat::o_LogPlayerTakeDamage
        )) {
            MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(logPlayerTakeDamageAddr));
            std::cout << "[GameHooks] Hooked LogPlayerTakeDamage at 0x" << std::hex << logPlayerTakeDamageAddr << std::endl;
        }

        std::cout << "[GameHooks] All hooks initialized successfully" << std::endl;
        g_initialized = true;
        return true;
    }

    bool Cleanup() {
        if (!g_initialized) {
            return true;
        }
        MinHookWrapper::DisableAllHooks();
        MinHookWrapper::Uninitialize();
        g_initialized = false;
        return true;
    }

    // Damage hook implementations
    namespace Damage {
        long hk_GetIncreaseValue_ShieldConversion(
            GameTypes::CombatCharacter* attacker,
            GameTypes::CombatCharacter* defender,
            GameTypes::SkillIdentity* attackSkill,
            bool isAttack,
            GameTypes::DamageInfo* damageInfo
        ) {
            long originalResult = o_GetIncreaseValue_ShieldConversion(attacker, defender, attackSkill, isAttack, damageInfo);

            if (g_features.infiniteDamage || g_features.oneHitKill) {
                return g_features.damageValue; // Custom damage value
            }

            return originalResult;
        }

        long hk_GetIncreaseValue_ShieldRemain(
            GameTypes::CombatCharacter* attacker,
            GameTypes::CombatCharacter* defender,
            GameTypes::SkillIdentity* attackSkill,
            bool isAttack,
            GameTypes::DamageInfo* damageInfo
        ) {
            long originalResult = o_GetIncreaseValue_ShieldRemain(attacker, defender, attackSkill, isAttack, damageInfo);

            if (g_features.infiniteDamage || g_features.oneHitKill) {
                return 999999999;
            }

            return originalResult;
        }

        long hk_GetAddDamage(GameTypes::SkillIdentity* attackSkill) {
            long originalResult = o_GetAddDamage(attackSkill);

            if (g_features.infiniteDamage || g_features.oneHitKill) {
                return 999999999;
            }

            return originalResult;
        }

        long hk_GetRate() {
            long originalResult = o_GetRate();

            if (g_features.superCrit) {
                return g_features.critRateValue; // Custom crit rate
            }

            return originalResult;
        }
    }

    // Stat hook implementations
    namespace Stats {
        void hk_AddStat(GameTypes::StatContainer* statContainer, int32_t statType, int value) {
            if (g_features.maxAttack && statType == static_cast<int32_t>(GameEnums::GuildStatType::AddAtt)) {
                o_AddStat(statContainer, statType, g_features.maxAttackValue);
                return;
            }

            if (g_features.maxDefense && statType == static_cast<int32_t>(GameEnums::GuildStatType::AddArm)) {
                o_AddStat(statContainer, statType, g_features.maxDefenseValue);
                return;
            }

            o_AddStat(statContainer, statType, value);
        }

        float hk_GetStatValue(GameTypes::StatContainer* statContainer) {
            float originalResult = o_GetStatValue(statContainer);

            // Modify stats based on features
            if (g_features.infiniteHP && originalResult < g_features.maxHPValue) {
                return g_features.maxHPValue;
            }

            if (g_features.infiniteMP && originalResult < g_features.maxMPValue) {
                return g_features.maxMPValue;
            }

            return originalResult;
        }

        void hk_SetStat(
            GameTypes::StatContainer* statContainer,
            int32_t statType,
            int64_t value1,
            int64_t value2,
            bool flag
        ) {
            o_SetStat(statContainer, statType, value1, value2, flag);
        }
    }

    // Hunter hook implementations
    namespace Hunter {
        void hk_SetHunterHP(int hp) {
            if (g_features.godMode || g_features.infiniteHP) {
                o_SetHunterHP(static_cast<int>(g_features.maxHPValue));
                return;
            }

            o_SetHunterHP(hp);
        }

        void hk_SetHunterActive(bool active) {
            o_SetHunterActive(active);
        }
    }

    // Cooldown hook implementations
    namespace Cooldown {
        void hk_OnSkillCoolTimeReduced() {
            if (g_features.noCooldown) {
                // Skip cooldown reduction - effectively no cooldown
                return;
            }

            o_OnSkillCoolTimeReduced();
        }

        void hk_ResetCoolTime() {
            if (g_features.noCooldown || g_features.instantSkill) {
                // Always reset cooldown
                o_ResetCoolTime();
                return;
            }

            o_ResetCoolTime();
        }
    }

    // Speed hook implementations
    namespace Speed {
        void hk_SetSpeed(float speed) {
            if (g_features.maxSpeed) {
                o_SetSpeed(g_features.speedMultiplier); // Custom speed multiplier
                return;
            }

            o_SetSpeed(speed);
        }
    }

    // Anti-Cheat hook implementations
    namespace AntiCheat {
        void hk_LogPlayerTakeDamage() {
            if (g_features.bypassAntiCheat) {
                // Skip logging to bypass anti-cheat
                return;
            }

            o_LogPlayerTakeDamage();
        }

        void hk_AntiCheatCommonPlayerTakeDamage() {
            if (g_features.bypassAntiCheat) {
                // Skip anti-cheat check
                return;
            }

            o_AntiCheatCommonPlayerTakeDamage();
        }
    }

    void ApplyStatModifiers() {
        // Apply stat modifications based on enabled features
        // This would be called periodically or when features are toggled
    }

    void ResetFeatures() {
        g_features = Features();
    }
}
