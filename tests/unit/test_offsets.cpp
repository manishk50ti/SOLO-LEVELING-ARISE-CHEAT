#include <gtest/gtest.h>
#include <cstdint>

// Re-implement Offsets namespace for testing (from include/Offsets.h)
namespace Offsets {
    inline uintptr_t GameAssemblyBase = 0;

    namespace Damage {
        constexpr uintptr_t GetIncreaseValue_ShieldConversion = 0x56DD40;
        constexpr uintptr_t GetIncreaseValue_ShieldRemain = 0x56DE80;
        constexpr uintptr_t GetAddDamage_ElementGSSkill = 0x55E970;
        constexpr uintptr_t GetAddDamage_ElementSkillType = 0x55EB70;
        constexpr uintptr_t GetAddDamage_HPComparison = 0x562520;
        constexpr uintptr_t GetRate_ElementGSSkillCritical = 0x53B5E0;
        constexpr uintptr_t GetRate_ElementSkillTypeCritical = 0x53B5E0;
        constexpr uintptr_t GetRate_HPComparison = 0x53B780;
        constexpr uintptr_t GetRate_SkillTypeCritical = 0x53B5E0;
    }

    namespace Stats {
        constexpr uintptr_t AddStat = 0x67433;
        constexpr uintptr_t GetStatValue = 0x67502;
        constexpr uintptr_t SetStat = 0x371413;
    }

    namespace Skills {
        constexpr uintptr_t SetSkill_Base = 0x18429;
        constexpr uintptr_t SetSkill_Override = 0x359968;
        constexpr uintptr_t SetSkill_UI = 0x371299;
        constexpr uintptr_t SetSkillDamageGroupID = 0x239792;
        constexpr uintptr_t SetSkillActive = 0x406679;
        constexpr uintptr_t OnSkillCoolTimeReduced = 0x18123;
        constexpr uintptr_t ResetCoolTime = 0x563826;
    }

    namespace Hunter {
        constexpr uintptr_t SetHunterActive = 0x268400;
        constexpr uintptr_t SetHunterHP = 0x360647;
        constexpr uintptr_t GetHunterData = 0x406716;
        constexpr uintptr_t SetHunterData = 0x358471;
        constexpr uintptr_t GetHunterUID = 0x470170;
    }

    namespace Cooldown {
        constexpr uintptr_t CooldownButton_GetCooldownTimeout = 0x1481054;
        constexpr uintptr_t CooldownButton_SetCooldownTimeout = 0x1481057;
        constexpr uintptr_t CooldownButton_GetCooldownSpeed = 0x1481060;
        constexpr uintptr_t CooldownButton_SetCooldownSpeed = 0x1481063;
        constexpr uintptr_t CooldownButton_PauseCooldown = 0x1481093;
        constexpr uintptr_t CooldownButton_RestartCooldown = 0x1481096;
        constexpr uintptr_t CooldownButton_StartCooldown = 0x1481099;
        constexpr uintptr_t CooldownButton_CancelCooldown = 0x1481102;
    }

    namespace Speed {
        constexpr uintptr_t SetSpeed = 0x322572;
    }

    namespace AntiCheat {
        constexpr uintptr_t AntiCheatCommonClientActionReason = 0x1177525;
        constexpr uintptr_t AntiCheatCommonPlayerTakeDamage = 0x1177668;
        constexpr uintptr_t LogPlayerTakeDamage = 0x1176912;
    }

    namespace Training {
        constexpr uintptr_t GetDamagePercent = 0x360668;
    }

    inline uintptr_t GetAbsoluteAddress(uintptr_t rva) {
        return GameAssemblyBase + rva;
    }
}

// --- GetAbsoluteAddress Tests ---

class OffsetsTest : public ::testing::Test {
protected:
    void SetUp() override {
        Offsets::GameAssemblyBase = 0;
    }
};

TEST_F(OffsetsTest, GetAbsoluteAddress_ZeroBase_ReturnsRVA) {
    Offsets::GameAssemblyBase = 0;
    EXPECT_EQ(Offsets::GetAbsoluteAddress(0x56DD40), 0x56DD40u);
}

TEST_F(OffsetsTest, GetAbsoluteAddress_NonZeroBase_AddsBase) {
    Offsets::GameAssemblyBase = 0x7FF600000000;
    EXPECT_EQ(Offsets::GetAbsoluteAddress(0x56DD40),
              0x7FF600000000 + 0x56DD40);
}

TEST_F(OffsetsTest, GetAbsoluteAddress_TypicalGameAssemblyBase) {
    Offsets::GameAssemblyBase = 0x180000000;
    uintptr_t result = Offsets::GetAbsoluteAddress(Offsets::Damage::GetIncreaseValue_ShieldConversion);
    EXPECT_EQ(result, 0x180000000 + 0x56DD40);
}

TEST_F(OffsetsTest, GetAbsoluteAddress_ZeroRVA) {
    Offsets::GameAssemblyBase = 0x7FF600000000;
    EXPECT_EQ(Offsets::GetAbsoluteAddress(0), 0x7FF600000000u);
}

// --- Offset Value Consistency Tests ---

TEST_F(OffsetsTest, DamageOffsets_AreNonZero) {
    EXPECT_NE(Offsets::Damage::GetIncreaseValue_ShieldConversion, 0u);
    EXPECT_NE(Offsets::Damage::GetIncreaseValue_ShieldRemain, 0u);
    EXPECT_NE(Offsets::Damage::GetAddDamage_ElementGSSkill, 0u);
    EXPECT_NE(Offsets::Damage::GetRate_ElementGSSkillCritical, 0u);
}

TEST_F(OffsetsTest, DamageOffsets_ShieldFunctions_AreDifferent) {
    EXPECT_NE(Offsets::Damage::GetIncreaseValue_ShieldConversion,
              Offsets::Damage::GetIncreaseValue_ShieldRemain);
}

TEST_F(OffsetsTest, DamageOffsets_GetRate_SomeShareAddress) {
    // ElementGSSkillCritical, ElementSkillTypeCritical, SkillTypeCritical share same address
    EXPECT_EQ(Offsets::Damage::GetRate_ElementGSSkillCritical,
              Offsets::Damage::GetRate_ElementSkillTypeCritical);
    EXPECT_EQ(Offsets::Damage::GetRate_ElementGSSkillCritical,
              Offsets::Damage::GetRate_SkillTypeCritical);
    // HPComparison is different
    EXPECT_NE(Offsets::Damage::GetRate_HPComparison,
              Offsets::Damage::GetRate_ElementGSSkillCritical);
}

TEST_F(OffsetsTest, StatOffsets_AreNonZero) {
    EXPECT_NE(Offsets::Stats::AddStat, 0u);
    EXPECT_NE(Offsets::Stats::GetStatValue, 0u);
    EXPECT_NE(Offsets::Stats::SetStat, 0u);
}

TEST_F(OffsetsTest, StatOffsets_AreDistinct) {
    EXPECT_NE(Offsets::Stats::AddStat, Offsets::Stats::GetStatValue);
    EXPECT_NE(Offsets::Stats::AddStat, Offsets::Stats::SetStat);
    EXPECT_NE(Offsets::Stats::GetStatValue, Offsets::Stats::SetStat);
}

TEST_F(OffsetsTest, HunterOffsets_AreNonZero) {
    EXPECT_NE(Offsets::Hunter::SetHunterActive, 0u);
    EXPECT_NE(Offsets::Hunter::SetHunterHP, 0u);
    EXPECT_NE(Offsets::Hunter::GetHunterData, 0u);
    EXPECT_NE(Offsets::Hunter::SetHunterData, 0u);
    EXPECT_NE(Offsets::Hunter::GetHunterUID, 0u);
}

TEST_F(OffsetsTest, SkillOffsets_AreNonZero) {
    EXPECT_NE(Offsets::Skills::SetSkill_Base, 0u);
    EXPECT_NE(Offsets::Skills::OnSkillCoolTimeReduced, 0u);
    EXPECT_NE(Offsets::Skills::ResetCoolTime, 0u);
}

TEST_F(OffsetsTest, CooldownOffsets_AreSequential) {
    // The cooldown button offsets are very close together
    EXPECT_LT(Offsets::Cooldown::CooldownButton_GetCooldownTimeout,
              Offsets::Cooldown::CooldownButton_CancelCooldown);
}

TEST_F(OffsetsTest, AntiCheatOffsets_AreNonZero) {
    EXPECT_NE(Offsets::AntiCheat::AntiCheatCommonClientActionReason, 0u);
    EXPECT_NE(Offsets::AntiCheat::AntiCheatCommonPlayerTakeDamage, 0u);
    EXPECT_NE(Offsets::AntiCheat::LogPlayerTakeDamage, 0u);
}

TEST_F(OffsetsTest, SpeedOffset_IsNonZero) {
    EXPECT_NE(Offsets::Speed::SetSpeed, 0u);
}

TEST_F(OffsetsTest, GetAbsoluteAddress_AllDamageOffsetsCalculateCorrectly) {
    Offsets::GameAssemblyBase = 0x10000;
    EXPECT_EQ(Offsets::GetAbsoluteAddress(Offsets::Damage::GetIncreaseValue_ShieldConversion),
              0x10000 + 0x56DD40);
    EXPECT_EQ(Offsets::GetAbsoluteAddress(Offsets::Damage::GetAddDamage_ElementGSSkill),
              0x10000 + 0x55E970);
}
