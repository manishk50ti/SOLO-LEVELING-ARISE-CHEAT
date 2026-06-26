#pragma once
#include "../include/Offsets.h"
#include "../include/Enums.h"
#include <cstdint>
#include <functional>

// Forward declarations for game types (obfuscated names from dump.cs)
namespace GameTypes {
    // OGDIOOHIFFG - combat character/actor (attacker/defender)
    struct CombatCharacter;

    // MPBGNEBPHGI - stat container
    struct StatContainer;

    // JHKPGKHJHAK - damage-info struct
    struct DamageInfo;

    // SkillIdentity - skill reference
    struct SkillIdentity;

    // CAPNPOLLELA - skill controller base
    struct SkillControllerBase;
}

namespace GameHooks {
    // Feature toggles
    struct Features {
        // Combat hacks
        bool godMode = false;
        bool oneHitKill = false;
        bool infiniteDamage = false;
        bool noCooldown = false;
        bool superCrit = false;
        bool instantSkill = false;

        // Stat modifiers
        bool infiniteHP = false;
        bool infiniteMP = false;
        bool maxAttack = false;
        bool maxDefense = false;
        bool maxSpeed = false;

        // Custom values
        int damageValue = 999999999;
        int critRateValue = 100;
        float speedMultiplier = 10.0f;
        float maxHPValue = 999999.0f;
        float maxMPValue = 999999.0f;
        int maxAttackValue = 999999;
        int maxDefenseValue = 999999;

        // Hunter features
        bool instantLevelUp = false;
        bool unlockAllHunters = false;
        bool maxShadowArmy = false;

        // Item/Currency
        bool infiniteGold = false;
        bool infiniteGems = false;
        bool infiniteAP = false;

        // Misc
        bool bypassAntiCheat = false;
        bool unlockAllContent = false;
        bool instantQuestComplete = false;
    };

    extern Features g_features;
    extern bool g_initialized;

    // Initialize all hooks
    bool Initialize();

    // Cleanup all hooks
    bool Cleanup();

    // Damage hooks
    namespace Damage {
        // GetIncreaseValue hook - modifies damage calculation
        using GetIncreaseValueFunc = long(*)(
            GameTypes::CombatCharacter* attacker,
            GameTypes::CombatCharacter* defender,
            GameTypes::SkillIdentity* attackSkill,
            bool isAttack,
            GameTypes::DamageInfo* damageInfo
        );

        extern GetIncreaseValueFunc o_GetIncreaseValue_ShieldConversion;
        extern GetIncreaseValueFunc o_GetIncreaseValue_ShieldRemain;

        long hk_GetIncreaseValue_ShieldConversion(
            GameTypes::CombatCharacter* attacker,
            GameTypes::CombatCharacter* defender,
            GameTypes::SkillIdentity* attackSkill,
            bool isAttack,
            GameTypes::DamageInfo* damageInfo
        );

        long hk_GetIncreaseValue_ShieldRemain(
            GameTypes::CombatCharacter* attacker,
            GameTypes::CombatCharacter* defender,
            GameTypes::SkillIdentity* attackSkill,
            bool isAttack,
            GameTypes::DamageInfo* damageInfo
        );

        // GetAddDamage hook
        using GetAddDamageFunc = long(*)(GameTypes::SkillIdentity* attackSkill);
        extern GetAddDamageFunc o_GetAddDamage;
        long hk_GetAddDamage(GameTypes::SkillIdentity* attackSkill);

        // GetRate hook - critical rate
        using GetRateFunc = long(*)();
        extern GetRateFunc o_GetRate;
        long hk_GetRate();
    }

    // Stat hooks
    namespace Stats {
        // AddStat hook
        using AddStatFunc = void(*)(GameTypes::StatContainer* statContainer, int32_t statType, int value);
        extern AddStatFunc o_AddStat;
        void hk_AddStat(GameTypes::StatContainer* statContainer, int32_t statType, int value);

        // GetStatValue hook
        using GetStatValueFunc = float(*)(GameTypes::StatContainer* statContainer);
        extern GetStatValueFunc o_GetStatValue;
        float hk_GetStatValue(GameTypes::StatContainer* statContainer);

        // SetStat hook
        using SetStatFunc = void(*)(
            GameTypes::StatContainer* statContainer,
            int32_t statType,
            int64_t value1,
            int64_t value2,
            bool flag
        );
        extern SetStatFunc o_SetStat;
        void hk_SetStat(
            GameTypes::StatContainer* statContainer,
            int32_t statType,
            int64_t value1,
            int64_t value2,
            bool flag
        );
    }

    // Hunter hooks
    namespace Hunter {
        // SetHunterHP hook
        using SetHunterHPFunc = void(*)(int hp);
        extern SetHunterHPFunc o_SetHunterHP;
        void hk_SetHunterHP(int hp);

        // SetHunterActive hook
        using SetHunterActiveFunc = void(*)(bool active);
        extern SetHunterActiveFunc o_SetHunterActive;
        void hk_SetHunterActive(bool active);
    }

    // Cooldown hooks
    namespace Cooldown {
        // OnSkillCoolTimeReduced hook
        using OnSkillCoolTimeReducedFunc = void(*)();
        extern OnSkillCoolTimeReducedFunc o_OnSkillCoolTimeReduced;
        void hk_OnSkillCoolTimeReduced();

        // ResetCoolTime hook
        using ResetCoolTimeFunc = void(*)();
        extern ResetCoolTimeFunc o_ResetCoolTime;
        void hk_ResetCoolTime();
    }

    // Speed hooks
    namespace Speed {
        // SetSpeed hook
        using SetSpeedFunc = void(*)(float speed);
        extern SetSpeedFunc o_SetSpeed;
        void hk_SetSpeed(float speed);
    }

    // Anti-Cheat hooks
    namespace AntiCheat {
        // LogPlayerTakeDamage hook
        using LogPlayerTakeDamageFunc = void(*)();
        extern LogPlayerTakeDamageFunc o_LogPlayerTakeDamage;
        void hk_LogPlayerTakeDamage();

        // AntiCheatCommonPlayerTakeDamage hook
        using AntiCheatCommonPlayerTakeDamageFunc = void(*)();
        extern AntiCheatCommonPlayerTakeDamageFunc o_AntiCheatCommonPlayerTakeDamage;
        void hk_AntiCheatCommonPlayerTakeDamage();
    }

    // Apply all stat modifications
    void ApplyStatModifiers();

    // Reset all features to default
    void ResetFeatures();
}
