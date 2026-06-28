#include "../include/GameHooks.h"
#include "../include/MinHookWrapper.h"
#include "../include/PatternScanner.h"
#include <Windows.h>
#include <thread>
#include <chrono>
#include <atomic>

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
            return false;
        }

        std::this_thread::sleep_for(std::chrono::seconds(10));

        Offsets::GameAssemblyBase = reinterpret_cast<uintptr_t>(gameAssembly);

        if (!MinHookWrapper::Initialize()) {
            return false;
        }

        auto hookAndEnable = [](uintptr_t rva, LPVOID detour, auto* original) -> bool {
            uintptr_t addr = Offsets::GetAbsoluteAddress(rva);
            if (!MinHookWrapper::CreateHook(
                reinterpret_cast<LPVOID>(addr), detour, original)) {
                return false;
            }
            return MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(addr));
        };

        hookAndEnable(Offsets::Damage::GetIncreaseValue_ShieldConversion,
            reinterpret_cast<LPVOID>(&Damage::hk_GetIncreaseValue_ShieldConversion),
            &Damage::o_GetIncreaseValue_ShieldConversion);

        hookAndEnable(Offsets::Damage::GetAddDamage_ElementGSSkill,
            reinterpret_cast<LPVOID>(&Damage::hk_GetAddDamage),
            &Damage::o_GetAddDamage);

        hookAndEnable(Offsets::Damage::GetRate_ElementGSSkillCritical,
            reinterpret_cast<LPVOID>(&Damage::hk_GetRate),
            &Damage::o_GetRate);

        hookAndEnable(Offsets::Stats::AddStat,
            reinterpret_cast<LPVOID>(&Stats::hk_AddStat),
            &Stats::o_AddStat);

        hookAndEnable(Offsets::Stats::GetStatValue,
            reinterpret_cast<LPVOID>(&Stats::hk_GetStatValue),
            &Stats::o_GetStatValue);

        hookAndEnable(Offsets::Hunter::SetHunterHP,
            reinterpret_cast<LPVOID>(&Hunter::hk_SetHunterHP),
            &Hunter::o_SetHunterHP);

        hookAndEnable(Offsets::Skills::OnSkillCoolTimeReduced,
            reinterpret_cast<LPVOID>(&Cooldown::hk_OnSkillCoolTimeReduced),
            &Cooldown::o_OnSkillCoolTimeReduced);

        hookAndEnable(Offsets::Speed::SetSpeed,
            reinterpret_cast<LPVOID>(&Speed::hk_SetSpeed),
            &Speed::o_SetSpeed);

        hookAndEnable(Offsets::AntiCheat::LogPlayerTakeDamage,
            reinterpret_cast<LPVOID>(&AntiCheat::hk_LogPlayerTakeDamage),
            &AntiCheat::o_LogPlayerTakeDamage);
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
            if (!o_GetIncreaseValue_ShieldConversion) return 0;
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
            if (!o_GetIncreaseValue_ShieldRemain) return 0;
            long originalResult = o_GetIncreaseValue_ShieldRemain(attacker, defender, attackSkill, isAttack, damageInfo);

            if (g_features.infiniteDamage || g_features.oneHitKill) {
                return 999999999;
            }

            return originalResult;
        }

        long hk_GetAddDamage(GameTypes::SkillIdentity* attackSkill) {
            if (!o_GetAddDamage) return 0;
            long originalResult = o_GetAddDamage(attackSkill);

            if (g_features.infiniteDamage || g_features.oneHitKill) {
                return 999999999;
            }

            return originalResult;
        }

        long hk_GetRate() {
            if (!o_GetRate) return 0;
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
            if (!o_AddStat) return;

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
            if (!o_GetStatValue) return 0.0f;
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
            if (!o_SetStat) return;
            o_SetStat(statContainer, statType, value1, value2, flag);
        }
    }

    // Hunter hook implementations
    namespace Hunter {
        void hk_SetHunterHP(int hp) {
            if (!o_SetHunterHP) return;

            if (g_features.godMode || g_features.infiniteHP) {
                o_SetHunterHP(static_cast<int>(g_features.maxHPValue));
                return;
            }

            o_SetHunterHP(hp);
        }

        void hk_SetHunterActive(bool active) {
            if (!o_SetHunterActive) return;
            o_SetHunterActive(active);
        }
    }

    // Cooldown hook implementations
    namespace Cooldown {
        void hk_OnSkillCoolTimeReduced() {
            if (g_features.noCooldown) {
                return;
            }

            if (o_OnSkillCoolTimeReduced) o_OnSkillCoolTimeReduced();
        }

        void hk_ResetCoolTime() {
            if (!o_ResetCoolTime) return;

            o_ResetCoolTime();
        }
    }

    // Speed hook implementations
    namespace Speed {
        void hk_SetSpeed(float speed) {
            if (!o_SetSpeed) return;

            if (g_features.maxSpeed) {
                o_SetSpeed(g_features.speedMultiplier);
                return;
            }

            o_SetSpeed(speed);
        }
    }

    // Anti-Cheat hook implementations
    namespace AntiCheat {
        void hk_LogPlayerTakeDamage() {
            if (g_features.bypassAntiCheat) {
                return;
            }

            if (o_LogPlayerTakeDamage) o_LogPlayerTakeDamage();
        }

        void hk_AntiCheatCommonPlayerTakeDamage() {
            if (g_features.bypassAntiCheat) {
                return;
            }

            if (o_AntiCheatCommonPlayerTakeDamage) o_AntiCheatCommonPlayerTakeDamage();
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
