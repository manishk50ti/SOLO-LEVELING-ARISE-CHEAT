#pragma once
#include <cstdint>

// Solo Leveling: Arise - Game Offsets
// Based on dump.cs analysis from Il2CppDumper

namespace Offsets {
    // Game Assembly Base (will be set at runtime)
    inline uintptr_t GameAssemblyBase = 0;

    // Image 0: Assembly-CSharp.dll - Main Game Assembly
    // RVA offsets from dump.cs

    // Damage Functions (from 01_combat_damage_stats.txt)
    namespace Damage {
        // ShieldConversionEnhanceDamage::GetIncreaseValue
        // RVA: 0x56DD40 Offset: 0x56C940
        constexpr uintptr_t GetIncreaseValue_ShieldConversion = 0x56DD40;

        // ShieldRemainConversionEnhanceDamage::GetIncreaseValue
        // RVA: 0x56DE80 Offset: 0x56CA80
        constexpr uintptr_t GetIncreaseValue_ShieldRemain = 0x56DE80;

        // GetAddDamage functions
        constexpr uintptr_t GetAddDamage_ElementGSSkill = 0x55E970;
        constexpr uintptr_t GetAddDamage_ElementSkillType = 0x55EB70;
        constexpr uintptr_t GetAddDamage_HPComparison = 0x562520;

        // GetRate functions
        constexpr uintptr_t GetRate_ElementGSSkillCritical = 0x53B5E0;
        constexpr uintptr_t GetRate_ElementSkillTypeCritical = 0x53B5E0;
        constexpr uintptr_t GetRate_HPComparison = 0x53B780;
        constexpr uintptr_t GetRate_SkillTypeCritical = 0x53B5E0;
    }

    // Stat Functions
    namespace Stats {
        // MPBGNEBPHGI::AddStat
        constexpr uintptr_t AddStat = 0x67433;

        // MPBGNEBPHGI::GetStatValue
        constexpr uintptr_t GetStatValue = 0x67502;

        // MPBGNEBPHGI::SetStat
        constexpr uintptr_t SetStat = 0x371413;
    }

    // Skill Functions (from 02_skills.txt)
    namespace Skills {
        // SetSkill functions
        constexpr uintptr_t SetSkill_Base = 0x18429;
        constexpr uintptr_t SetSkill_Override = 0x359968;
        constexpr uintptr_t SetSkill_UI = 0x371299;
        constexpr uintptr_t SetSkillDamageGroupID = 0x239792;
        constexpr uintptr_t SetSkillActive = 0x406679;

        // Skill cooldown functions
        constexpr uintptr_t OnSkillCoolTimeReduced = 0x18123;
        constexpr uintptr_t ResetCoolTime = 0x563826;
    }

    // Hunter Functions (from 03_hunters_shadows.txt)
    namespace Hunter {
        constexpr uintptr_t SetHunterActive = 0x268400;
        constexpr uintptr_t SetHunterHP = 0x360647;
        constexpr uintptr_t GetHunterData = 0x406716;
        constexpr uintptr_t SetHunterData = 0x358471;
        constexpr uintptr_t GetHunterUID = 0x470170;
    }

    // Cooldown Functions (from 04_cooldown_move_buffs.txt)
    namespace Cooldown {
        // CooldownButton functions
        constexpr uintptr_t CooldownButton_GetCooldownTimeout = 0x1481054;
        constexpr uintptr_t CooldownButton_SetCooldownTimeout = 0x1481057;
        constexpr uintptr_t CooldownButton_GetCooldownSpeed = 0x1481060;
        constexpr uintptr_t CooldownButton_SetCooldownSpeed = 0x1481063;
        constexpr uintptr_t CooldownButton_PauseCooldown = 0x1481093;
        constexpr uintptr_t CooldownButton_RestartCooldown = 0x1481096;
        constexpr uintptr_t CooldownButton_StartCooldown = 0x1481099;
        constexpr uintptr_t CooldownButton_CancelCooldown = 0x1481102;
    }

    // Speed Functions
    namespace Speed {
        constexpr uintptr_t SetSpeed = 0x322572;
    }

    // Anti-Cheat Functions (from README)
    namespace AntiCheat {
        constexpr uintptr_t AntiCheatCommonClientActionReason = 0x1177525;
        constexpr uintptr_t AntiCheatCommonPlayerTakeDamage = 0x1177668;
        constexpr uintptr_t LogPlayerTakeDamage = 0x1176912;
    }

    // Training Room Functions (from 20_training_special.txt)
    namespace Training {
        constexpr uintptr_t GetDamagePercent = 0x360668;
    }

    // Helper to get absolute address
    inline uintptr_t GetAbsoluteAddress(uintptr_t rva) {
        return GameAssemblyBase + rva;
    }
}
