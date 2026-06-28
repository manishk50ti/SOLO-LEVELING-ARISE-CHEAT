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

        int hooksCreated = 0;
        int hooksFailed = 0;

        auto installHook = [&](uintptr_t rva, LPVOID detour, auto* original, const char* name) {
            uintptr_t addr = Offsets::GetAbsoluteAddress(rva);
            if (MinHookWrapper::CreateHook(reinterpret_cast<LPVOID>(addr), detour, original)) {
                if (MinHookWrapper::EnableHook(reinterpret_cast<LPVOID>(addr))) {
                    std::cout << "[GameHooks] Hooked " << name << " at 0x" << std::hex << addr << std::endl;
                    hooksCreated++;
                } else {
                    std::cerr << "[GameHooks] Failed to enable hook: " << name << " at 0x" << std::hex << addr << std::endl;
                    hooksFailed++;
                }
            } else {
                std::cerr << "[GameHooks] Failed to create hook: " << name << " at 0x" << std::hex << addr << std::endl;
                hooksFailed++;
            }
        };

        installHook(Offsets::Damage::GetIncreaseValue_ShieldConversion,
            reinterpret_cast<LPVOID>(&Damage::hk_GetIncreaseValue_ShieldConversion),
            &Damage::o_GetIncreaseValue_ShieldConversion,
            "GetIncreaseValue_ShieldConversion");

        installHook(Offsets::Damage::GetAddDamage_ElementGSSkill,
            reinterpret_cast<LPVOID>(&Damage::hk_GetAddDamage),
            &Damage::o_GetAddDamage,
            "GetAddDamage");

        installHook(Offsets::Damage::GetRate_ElementGSSkillCritical,
            reinterpret_cast<LPVOID>(&Damage::hk_GetRate),
            &Damage::o_GetRate,
            "GetRate");

        installHook(Offsets::Stats::AddStat,
            reinterpret_cast<LPVOID>(&Stats::hk_AddStat),
            &Stats::o_AddStat,
            "AddStat");

        installHook(Offsets::Stats::GetStatValue,
            reinterpret_cast<LPVOID>(&Stats::hk_GetStatValue),
            &Stats::o_GetStatValue,
            "GetStatValue");

        installHook(Offsets::Hunter::SetHunterHP,
            reinterpret_cast<LPVOID>(&Hunter::hk_SetHunterHP),
            &Hunter::o_SetHunterHP,
            "SetHunterHP");

        installHook(Offsets::Skills::OnSkillCoolTimeReduced,
            reinterpret_cast<LPVOID>(&Cooldown::hk_OnSkillCoolTimeReduced),
            &Cooldown::o_OnSkillCoolTimeReduced,
            "OnSkillCoolTimeReduced");

        installHook(Offsets::Speed::SetSpeed,
            reinterpret_cast<LPVOID>(&Speed::hk_SetSpeed),
            &Speed::o_SetSpeed,
            "SetSpeed");

        installHook(Offsets::AntiCheat::LogPlayerTakeDamage,
            reinterpret_cast<LPVOID>(&AntiCheat::hk_LogPlayerTakeDamage),
            &AntiCheat::o_LogPlayerTakeDamage,
            "LogPlayerTakeDamage");

        if (hooksFailed > 0) {
            std::cerr << "[GameHooks] " << std::dec << hooksFailed << " hook(s) failed, "
                      << hooksCreated << " hook(s) active" << std::endl;
        } else {
            std::cout << "[GameHooks] All " << std::dec << hooksCreated << " hooks initialized successfully" << std::endl;
        }

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
                return g_features.damageValue;
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
                return g_features.critRateValue;
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
