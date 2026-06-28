#include <gtest/gtest.h>
#include <cstdint>
#include <cstring>

// Re-declare the Feature struct and hook logic for testing
// without Windows/game dependencies

namespace GameEnums {
    enum class GuildStatType : int32_t {
        AddAtt = 0,
        AddArm = 1,
        AddMHP = 2,
        AAddMMP = 3,
        Critical = 4,
        CriDam = 5,
    };
}

namespace GameTypes {
    struct CombatCharacter {};
    struct StatContainer {};
    struct DamageInfo {};
    struct SkillIdentity {};
}

struct Features {
    bool godMode = false;
    bool oneHitKill = false;
    bool infiniteDamage = false;
    bool noCooldown = false;
    bool superCrit = false;
    bool instantSkill = false;

    bool infiniteHP = false;
    bool infiniteMP = false;
    bool maxAttack = false;
    bool maxDefense = false;
    bool maxSpeed = false;

    int damageValue = 999999999;
    int critRateValue = 100;
    float speedMultiplier = 10.0f;
    float maxHPValue = 999999.0f;
    float maxMPValue = 999999.0f;
    int maxAttackValue = 999999;
    int maxDefenseValue = 999999;

    bool instantLevelUp = false;
    bool unlockAllHunters = false;
    bool maxShadowArmy = false;

    bool infiniteGold = false;
    bool infiniteGems = false;
    bool infiniteAP = false;

    bool bypassAntiCheat = false;
    bool unlockAllContent = false;
    bool instantQuestComplete = false;
};

// Simulated hook logic extracted from GameHooks.cpp
namespace TestableHooks {

static Features g_features;
static long g_originalDamageResult = 100;
static float g_originalStatValue = 50.0f;
static int g_lastSetHP = 0;
static float g_lastSetSpeed = 0.0f;
static bool g_cooldownCalled = false;
static int g_lastStatType = -1;
static int g_lastStatValue = 0;

void Reset() {
    g_features = Features();
    g_originalDamageResult = 100;
    g_originalStatValue = 50.0f;
    g_lastSetHP = 0;
    g_lastSetSpeed = 0.0f;
    g_cooldownCalled = false;
    g_lastStatType = -1;
    g_lastStatValue = 0;
}

long hk_GetIncreaseValue_ShieldConversion(long originalResult) {
    if (g_features.infiniteDamage || g_features.oneHitKill) {
        return g_features.damageValue;
    }
    return originalResult;
}

long hk_GetIncreaseValue_ShieldRemain(long originalResult) {
    if (g_features.infiniteDamage || g_features.oneHitKill) {
        return 999999999;
    }
    return originalResult;
}

long hk_GetAddDamage(long originalResult) {
    if (g_features.infiniteDamage || g_features.oneHitKill) {
        return 999999999;
    }
    return originalResult;
}

long hk_GetRate(long originalResult) {
    if (g_features.superCrit) {
        return g_features.critRateValue;
    }
    return originalResult;
}

float hk_GetStatValue(float originalResult) {
    if (g_features.infiniteHP && originalResult < g_features.maxHPValue) {
        return g_features.maxHPValue;
    }
    if (g_features.infiniteMP && originalResult < g_features.maxMPValue) {
        return g_features.maxMPValue;
    }
    return originalResult;
}

void hk_AddStat(int32_t statType, int value) {
    if (g_features.maxAttack && statType == static_cast<int32_t>(GameEnums::GuildStatType::AddAtt)) {
        g_lastStatType = statType;
        g_lastStatValue = g_features.maxAttackValue;
        return;
    }
    if (g_features.maxDefense && statType == static_cast<int32_t>(GameEnums::GuildStatType::AddArm)) {
        g_lastStatType = statType;
        g_lastStatValue = g_features.maxDefenseValue;
        return;
    }
    g_lastStatType = statType;
    g_lastStatValue = value;
}

void hk_SetHunterHP(int hp) {
    if (g_features.godMode || g_features.infiniteHP) {
        g_lastSetHP = static_cast<int>(g_features.maxHPValue);
        return;
    }
    g_lastSetHP = hp;
}

bool hk_OnSkillCoolTimeReduced() {
    if (g_features.noCooldown) {
        return false; // skipped
    }
    g_cooldownCalled = true;
    return true; // called original
}

void hk_SetSpeed(float speed) {
    if (g_features.maxSpeed) {
        g_lastSetSpeed = g_features.speedMultiplier;
        return;
    }
    g_lastSetSpeed = speed;
}

bool hk_LogPlayerTakeDamage() {
    if (g_features.bypassAntiCheat) {
        return false; // skipped
    }
    return true; // called original
}

} // namespace TestableHooks

// --- Feature Default Values Tests ---

class FeaturesDefaultTest : public ::testing::Test {
protected:
    Features features;
};

TEST_F(FeaturesDefaultTest, AllBooleansDefaultToFalse) {
    EXPECT_FALSE(features.godMode);
    EXPECT_FALSE(features.oneHitKill);
    EXPECT_FALSE(features.infiniteDamage);
    EXPECT_FALSE(features.noCooldown);
    EXPECT_FALSE(features.superCrit);
    EXPECT_FALSE(features.instantSkill);
    EXPECT_FALSE(features.infiniteHP);
    EXPECT_FALSE(features.infiniteMP);
    EXPECT_FALSE(features.maxAttack);
    EXPECT_FALSE(features.maxDefense);
    EXPECT_FALSE(features.maxSpeed);
    EXPECT_FALSE(features.instantLevelUp);
    EXPECT_FALSE(features.unlockAllHunters);
    EXPECT_FALSE(features.maxShadowArmy);
    EXPECT_FALSE(features.infiniteGold);
    EXPECT_FALSE(features.infiniteGems);
    EXPECT_FALSE(features.infiniteAP);
    EXPECT_FALSE(features.bypassAntiCheat);
    EXPECT_FALSE(features.unlockAllContent);
    EXPECT_FALSE(features.instantQuestComplete);
}

TEST_F(FeaturesDefaultTest, DefaultValuesAreCorrect) {
    EXPECT_EQ(features.damageValue, 999999999);
    EXPECT_EQ(features.critRateValue, 100);
    EXPECT_FLOAT_EQ(features.speedMultiplier, 10.0f);
    EXPECT_FLOAT_EQ(features.maxHPValue, 999999.0f);
    EXPECT_FLOAT_EQ(features.maxMPValue, 999999.0f);
    EXPECT_EQ(features.maxAttackValue, 999999);
    EXPECT_EQ(features.maxDefenseValue, 999999);
}

// --- Damage Hook Logic Tests ---

class DamageHookTest : public ::testing::Test {
protected:
    void SetUp() override {
        TestableHooks::Reset();
    }
};

TEST_F(DamageHookTest, ShieldConversion_NoFeatures_ReturnsOriginal) {
    long result = TestableHooks::hk_GetIncreaseValue_ShieldConversion(100);
    EXPECT_EQ(result, 100);
}

TEST_F(DamageHookTest, ShieldConversion_InfiniteDamage_ReturnsCustomValue) {
    TestableHooks::g_features.infiniteDamage = true;
    long result = TestableHooks::hk_GetIncreaseValue_ShieldConversion(100);
    EXPECT_EQ(result, 999999999);
}

TEST_F(DamageHookTest, ShieldConversion_OneHitKill_ReturnsCustomValue) {
    TestableHooks::g_features.oneHitKill = true;
    long result = TestableHooks::hk_GetIncreaseValue_ShieldConversion(100);
    EXPECT_EQ(result, 999999999);
}

TEST_F(DamageHookTest, ShieldConversion_CustomDamageValue) {
    TestableHooks::g_features.infiniteDamage = true;
    TestableHooks::g_features.damageValue = 555;
    long result = TestableHooks::hk_GetIncreaseValue_ShieldConversion(100);
    EXPECT_EQ(result, 555);
}

TEST_F(DamageHookTest, ShieldRemain_NoFeatures_ReturnsOriginal) {
    long result = TestableHooks::hk_GetIncreaseValue_ShieldRemain(50);
    EXPECT_EQ(result, 50);
}

TEST_F(DamageHookTest, ShieldRemain_InfiniteDamage_ReturnsMax) {
    TestableHooks::g_features.infiniteDamage = true;
    long result = TestableHooks::hk_GetIncreaseValue_ShieldRemain(50);
    EXPECT_EQ(result, 999999999);
}

TEST_F(DamageHookTest, GetAddDamage_NoFeatures_ReturnsOriginal) {
    long result = TestableHooks::hk_GetAddDamage(200);
    EXPECT_EQ(result, 200);
}

TEST_F(DamageHookTest, GetAddDamage_OneHitKill_ReturnsMax) {
    TestableHooks::g_features.oneHitKill = true;
    long result = TestableHooks::hk_GetAddDamage(200);
    EXPECT_EQ(result, 999999999);
}

TEST_F(DamageHookTest, GetRate_NoFeatures_ReturnsOriginal) {
    long result = TestableHooks::hk_GetRate(50);
    EXPECT_EQ(result, 50);
}

TEST_F(DamageHookTest, GetRate_SuperCrit_ReturnsCustomRate) {
    TestableHooks::g_features.superCrit = true;
    long result = TestableHooks::hk_GetRate(50);
    EXPECT_EQ(result, 100);
}

TEST_F(DamageHookTest, GetRate_SuperCrit_CustomValue) {
    TestableHooks::g_features.superCrit = true;
    TestableHooks::g_features.critRateValue = 75;
    long result = TestableHooks::hk_GetRate(50);
    EXPECT_EQ(result, 75);
}

// --- Stat Hook Logic Tests ---

class StatHookTest : public ::testing::Test {
protected:
    void SetUp() override {
        TestableHooks::Reset();
    }
};

TEST_F(StatHookTest, GetStatValue_NoFeatures_ReturnsOriginal) {
    float result = TestableHooks::hk_GetStatValue(100.0f);
    EXPECT_FLOAT_EQ(result, 100.0f);
}

TEST_F(StatHookTest, GetStatValue_InfiniteHP_BelowMax_ReturnsMax) {
    TestableHooks::g_features.infiniteHP = true;
    float result = TestableHooks::hk_GetStatValue(100.0f);
    EXPECT_FLOAT_EQ(result, 999999.0f);
}

TEST_F(StatHookTest, GetStatValue_InfiniteHP_AboveMax_ReturnsOriginal) {
    TestableHooks::g_features.infiniteHP = true;
    TestableHooks::g_features.maxHPValue = 500.0f;
    float result = TestableHooks::hk_GetStatValue(600.0f);
    EXPECT_FLOAT_EQ(result, 600.0f);
}

TEST_F(StatHookTest, GetStatValue_InfiniteMP_BelowMax_ReturnsMax) {
    TestableHooks::g_features.infiniteMP = true;
    float result = TestableHooks::hk_GetStatValue(100.0f);
    EXPECT_FLOAT_EQ(result, 999999.0f);
}

TEST_F(StatHookTest, GetStatValue_InfiniteHP_Priority_OverMP) {
    TestableHooks::g_features.infiniteHP = true;
    TestableHooks::g_features.infiniteMP = true;
    TestableHooks::g_features.maxHPValue = 1000.0f;
    TestableHooks::g_features.maxMPValue = 2000.0f;
    // When HP is below maxHP, HP takes priority
    float result = TestableHooks::hk_GetStatValue(500.0f);
    EXPECT_FLOAT_EQ(result, 1000.0f);
}

TEST_F(StatHookTest, AddStat_NoFeatures_PassesThrough) {
    TestableHooks::hk_AddStat(static_cast<int32_t>(GameEnums::GuildStatType::AddAtt), 50);
    EXPECT_EQ(TestableHooks::g_lastStatType, 0);
    EXPECT_EQ(TestableHooks::g_lastStatValue, 50);
}

TEST_F(StatHookTest, AddStat_MaxAttack_OverridesAttackValue) {
    TestableHooks::g_features.maxAttack = true;
    TestableHooks::hk_AddStat(static_cast<int32_t>(GameEnums::GuildStatType::AddAtt), 50);
    EXPECT_EQ(TestableHooks::g_lastStatValue, 999999);
}

TEST_F(StatHookTest, AddStat_MaxDefense_OverridesDefenseValue) {
    TestableHooks::g_features.maxDefense = true;
    TestableHooks::hk_AddStat(static_cast<int32_t>(GameEnums::GuildStatType::AddArm), 50);
    EXPECT_EQ(TestableHooks::g_lastStatValue, 999999);
}

TEST_F(StatHookTest, AddStat_MaxAttack_DoesNotAffectOtherStats) {
    TestableHooks::g_features.maxAttack = true;
    TestableHooks::hk_AddStat(static_cast<int32_t>(GameEnums::GuildStatType::AddArm), 50);
    EXPECT_EQ(TestableHooks::g_lastStatValue, 50);
}

TEST_F(StatHookTest, AddStat_MaxDefense_DoesNotAffectAttack) {
    TestableHooks::g_features.maxDefense = true;
    TestableHooks::hk_AddStat(static_cast<int32_t>(GameEnums::GuildStatType::AddAtt), 50);
    EXPECT_EQ(TestableHooks::g_lastStatValue, 50);
}

// --- Hunter Hook Logic Tests ---

class HunterHookTest : public ::testing::Test {
protected:
    void SetUp() override {
        TestableHooks::Reset();
    }
};

TEST_F(HunterHookTest, SetHunterHP_NoFeatures_SetsOriginal) {
    TestableHooks::hk_SetHunterHP(500);
    EXPECT_EQ(TestableHooks::g_lastSetHP, 500);
}

TEST_F(HunterHookTest, SetHunterHP_GodMode_SetsMax) {
    TestableHooks::g_features.godMode = true;
    TestableHooks::hk_SetHunterHP(500);
    EXPECT_EQ(TestableHooks::g_lastSetHP, static_cast<int>(999999.0f));
}

TEST_F(HunterHookTest, SetHunterHP_InfiniteHP_SetsMax) {
    TestableHooks::g_features.infiniteHP = true;
    TestableHooks::hk_SetHunterHP(500);
    EXPECT_EQ(TestableHooks::g_lastSetHP, static_cast<int>(999999.0f));
}

TEST_F(HunterHookTest, SetHunterHP_GodMode_CustomMaxHP) {
    TestableHooks::g_features.godMode = true;
    TestableHooks::g_features.maxHPValue = 5000.0f;
    TestableHooks::hk_SetHunterHP(100);
    EXPECT_EQ(TestableHooks::g_lastSetHP, 5000);
}

// --- Cooldown Hook Logic Tests ---

class CooldownHookTest : public ::testing::Test {
protected:
    void SetUp() override {
        TestableHooks::Reset();
    }
};

TEST_F(CooldownHookTest, OnSkillCoolTimeReduced_NoFeatures_CallsOriginal) {
    bool called = TestableHooks::hk_OnSkillCoolTimeReduced();
    EXPECT_TRUE(called);
    EXPECT_TRUE(TestableHooks::g_cooldownCalled);
}

TEST_F(CooldownHookTest, OnSkillCoolTimeReduced_NoCooldown_SkipsOriginal) {
    TestableHooks::g_features.noCooldown = true;
    bool called = TestableHooks::hk_OnSkillCoolTimeReduced();
    EXPECT_FALSE(called);
    EXPECT_FALSE(TestableHooks::g_cooldownCalled);
}

// --- Speed Hook Logic Tests ---

class SpeedHookTest : public ::testing::Test {
protected:
    void SetUp() override {
        TestableHooks::Reset();
    }
};

TEST_F(SpeedHookTest, SetSpeed_NoFeatures_SetsOriginal) {
    TestableHooks::hk_SetSpeed(1.0f);
    EXPECT_FLOAT_EQ(TestableHooks::g_lastSetSpeed, 1.0f);
}

TEST_F(SpeedHookTest, SetSpeed_MaxSpeed_SetsMultiplier) {
    TestableHooks::g_features.maxSpeed = true;
    TestableHooks::hk_SetSpeed(1.0f);
    EXPECT_FLOAT_EQ(TestableHooks::g_lastSetSpeed, 10.0f);
}

TEST_F(SpeedHookTest, SetSpeed_MaxSpeed_CustomMultiplier) {
    TestableHooks::g_features.maxSpeed = true;
    TestableHooks::g_features.speedMultiplier = 25.0f;
    TestableHooks::hk_SetSpeed(1.0f);
    EXPECT_FLOAT_EQ(TestableHooks::g_lastSetSpeed, 25.0f);
}

// --- Anti-Cheat Hook Logic Tests ---

class AntiCheatHookTest : public ::testing::Test {
protected:
    void SetUp() override {
        TestableHooks::Reset();
    }
};

TEST_F(AntiCheatHookTest, LogPlayerTakeDamage_NoBypass_CallsOriginal) {
    bool called = TestableHooks::hk_LogPlayerTakeDamage();
    EXPECT_TRUE(called);
}

TEST_F(AntiCheatHookTest, LogPlayerTakeDamage_Bypass_SkipsOriginal) {
    TestableHooks::g_features.bypassAntiCheat = true;
    bool called = TestableHooks::hk_LogPlayerTakeDamage();
    EXPECT_FALSE(called);
}

// --- ResetFeatures Test ---

class ResetFeaturesTest : public ::testing::Test {
protected:
    void SetUp() override {
        TestableHooks::Reset();
    }
};

TEST_F(ResetFeaturesTest, ResetClearsAllFeatures) {
    TestableHooks::g_features.godMode = true;
    TestableHooks::g_features.infiniteDamage = true;
    TestableHooks::g_features.maxSpeed = true;
    TestableHooks::g_features.damageValue = 555;

    TestableHooks::Reset();

    EXPECT_FALSE(TestableHooks::g_features.godMode);
    EXPECT_FALSE(TestableHooks::g_features.infiniteDamage);
    EXPECT_FALSE(TestableHooks::g_features.maxSpeed);
    EXPECT_EQ(TestableHooks::g_features.damageValue, 999999999);
}
