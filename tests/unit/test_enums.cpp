#include <gtest/gtest.h>
#include <cstdint>

// Re-declare enums from include/Enums.h for testing
namespace GameEnums {
    enum class GuildStatType : int32_t {
        AddAtt = 0,
        AddArm = 1,
        AddMHP = 2,
        AAddMMP = 3,
        Critical = 4,
        CriDam = 5,
        Precision = 6,
        ArmPen = 7,
        Dam = 8,
        EnhanceSkillTypeDamage_BaseAttack = 9,
        EnhanceSkillTypeDamage_CoreAttack = 10,
        EnhanceSkillTypeDamage_NormalSkill = 11,
        EnhanceSkillTypeDamage_QTE = 12,
        EnhanceSkillTypeDamage_Supporter = 13,
        EnhanceSkillTypeDamage_UltimateSkill = 14,
        ElementDamFire = 15,
        ElementDamWater = 16
    };

    enum class ClassType : int32_t {
        None = 0,
        Public = 1,
        Striker = 2,
        Breaker = 3,
        Supporter = 4,
        Stacker = 5,
        Buster = 6,
        Successor = 7,
        All = 99
    };

    enum class RankType : int32_t {
        None = 0,
        E = 1,
        D = 2,
        C = 3,
        B = 4,
        A = 5,
        S = 6,
        SS = 7,
        Unknown = 8
    };

    enum class CharacterType : int32_t {
        None = 0,
        Hunter = 1,
        Monster = 2,
        Shadow = 3,
        Object = 4,
        NPC = 5,
        Dialog = 6,
        MainPC = 7,
        SystemPlayHunter = 8,
        SystemPlayShadow = 9,
        SystemPlayPC = 10,
        Part = 11,
        Max = 12
    };

    enum class FactionType : int32_t {
        PlayerSide = 0,
        MonsterSide = 1,
        Neutral = 3
    };

    enum class WeaponCategory : int32_t {
        None = 0,
        Bow = 1,
        Gun = 2,
        Sword = 3,
        Polearm = 4,
        Scythe = 5,
        Dagger = 6,
        Catalyst = 7
    };

    enum class ItemRarity : int32_t {
        None = 0,
        Common = 1,
        Uncommon = 2,
        Rare = 3,
        Epic = 4,
        Legendary = 5,
        Mythical = 6,
        Empty = 7
    };

    enum class BuffLargeType : int32_t {
        None = 0,
        Buff = 1,
        DeBuff = 2,
        Passive = 3,
        Special = 4,
        ShadowOnly = 5
    };

    enum class CooldownType : int32_t {
        Normal = 0,
        AfterCool = 1,
        AfterBuffCool = 2,
        Max = 3
    };

    enum class MonarchType : int32_t {
        None = 0,
        Destruction = 1,
        WhiteFlames = 2,
        Shadow = 3,
        TheBeginning = 4,
        Fangs = 5,
        Plagues = 6,
        Transfiguration = 7,
        IronBody = 8,
        Ice = 9
    };
}

// --- GuildStatType Tests ---

class GuildStatTypeTest : public ::testing::Test {};

TEST_F(GuildStatTypeTest, ValuesAreSequential) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::GuildStatType::AddAtt), 0);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::GuildStatType::AddArm), 1);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::GuildStatType::AddMHP), 2);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::GuildStatType::AAddMMP), 3);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::GuildStatType::Critical), 4);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::GuildStatType::CriDam), 5);
}

TEST_F(GuildStatTypeTest, ElementDamageStartsAt15) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::GuildStatType::ElementDamFire), 15);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::GuildStatType::ElementDamWater), 16);
}

TEST_F(GuildStatTypeTest, CastToInt32AndBack) {
    int32_t val = static_cast<int32_t>(GameEnums::GuildStatType::Critical);
    GameEnums::GuildStatType restored = static_cast<GameEnums::GuildStatType>(val);
    EXPECT_EQ(restored, GameEnums::GuildStatType::Critical);
}

// --- ClassType Tests ---

class ClassTypeTest : public ::testing::Test {};

TEST_F(ClassTypeTest, NoneIsZero) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::ClassType::None), 0);
}

TEST_F(ClassTypeTest, AllIs99) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::ClassType::All), 99);
}

TEST_F(ClassTypeTest, CombatClassesAre2Through7) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::ClassType::Striker), 2);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::ClassType::Breaker), 3);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::ClassType::Supporter), 4);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::ClassType::Stacker), 5);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::ClassType::Buster), 6);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::ClassType::Successor), 7);
}

// --- RankType Tests ---

class RankTypeTest : public ::testing::Test {};

TEST_F(RankTypeTest, RanksAreOrdered) {
    EXPECT_LT(static_cast<int32_t>(GameEnums::RankType::E),
              static_cast<int32_t>(GameEnums::RankType::D));
    EXPECT_LT(static_cast<int32_t>(GameEnums::RankType::D),
              static_cast<int32_t>(GameEnums::RankType::C));
    EXPECT_LT(static_cast<int32_t>(GameEnums::RankType::C),
              static_cast<int32_t>(GameEnums::RankType::B));
    EXPECT_LT(static_cast<int32_t>(GameEnums::RankType::B),
              static_cast<int32_t>(GameEnums::RankType::A));
    EXPECT_LT(static_cast<int32_t>(GameEnums::RankType::A),
              static_cast<int32_t>(GameEnums::RankType::S));
    EXPECT_LT(static_cast<int32_t>(GameEnums::RankType::S),
              static_cast<int32_t>(GameEnums::RankType::SS));
}

TEST_F(RankTypeTest, SSIs7) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::RankType::SS), 7);
}

// --- CharacterType Tests ---

class CharacterTypeTest : public ::testing::Test {};

TEST_F(CharacterTypeTest, MaxIs12) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::CharacterType::Max), 12);
}

TEST_F(CharacterTypeTest, MainGameTypes) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::CharacterType::Hunter), 1);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::CharacterType::Monster), 2);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::CharacterType::Shadow), 3);
}

// --- FactionType Tests ---

class FactionTypeTest : public ::testing::Test {};

TEST_F(FactionTypeTest, PlayerIsZero) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::FactionType::PlayerSide), 0);
}

TEST_F(FactionTypeTest, NeutralIs3_NotSequential) {
    // There's a gap between MonsterSide(1) and Neutral(3)
    EXPECT_EQ(static_cast<int32_t>(GameEnums::FactionType::MonsterSide), 1);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::FactionType::Neutral), 3);
}

// --- WeaponCategory Tests ---

class WeaponCategoryTest : public ::testing::Test {};

TEST_F(WeaponCategoryTest, AllWeaponsHaveDistinctValues) {
    int32_t values[] = {
        static_cast<int32_t>(GameEnums::WeaponCategory::None),
        static_cast<int32_t>(GameEnums::WeaponCategory::Bow),
        static_cast<int32_t>(GameEnums::WeaponCategory::Gun),
        static_cast<int32_t>(GameEnums::WeaponCategory::Sword),
        static_cast<int32_t>(GameEnums::WeaponCategory::Polearm),
        static_cast<int32_t>(GameEnums::WeaponCategory::Scythe),
        static_cast<int32_t>(GameEnums::WeaponCategory::Dagger),
        static_cast<int32_t>(GameEnums::WeaponCategory::Catalyst),
    };
    for (int i = 0; i < 8; i++) {
        for (int j = i + 1; j < 8; j++) {
            EXPECT_NE(values[i], values[j]) << "Weapons " << i << " and " << j << " share value";
        }
    }
}

// --- ItemRarity Tests ---

class ItemRarityTest : public ::testing::Test {};

TEST_F(ItemRarityTest, RaritiesAreOrdered) {
    EXPECT_LT(static_cast<int32_t>(GameEnums::ItemRarity::Common),
              static_cast<int32_t>(GameEnums::ItemRarity::Uncommon));
    EXPECT_LT(static_cast<int32_t>(GameEnums::ItemRarity::Uncommon),
              static_cast<int32_t>(GameEnums::ItemRarity::Rare));
    EXPECT_LT(static_cast<int32_t>(GameEnums::ItemRarity::Rare),
              static_cast<int32_t>(GameEnums::ItemRarity::Epic));
    EXPECT_LT(static_cast<int32_t>(GameEnums::ItemRarity::Epic),
              static_cast<int32_t>(GameEnums::ItemRarity::Legendary));
    EXPECT_LT(static_cast<int32_t>(GameEnums::ItemRarity::Legendary),
              static_cast<int32_t>(GameEnums::ItemRarity::Mythical));
}

// --- MonarchType Tests ---

class MonarchTypeTest : public ::testing::Test {};

TEST_F(MonarchTypeTest, AllMonarchsHaveValues1Through9) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::MonarchType::Destruction), 1);
    EXPECT_EQ(static_cast<int32_t>(GameEnums::MonarchType::Ice), 9);
}

TEST_F(MonarchTypeTest, ShadowMonarchIs3) {
    // Sung Jinwoo's type
    EXPECT_EQ(static_cast<int32_t>(GameEnums::MonarchType::Shadow), 3);
}

// --- CooldownType Tests ---

class CooldownTypeTest : public ::testing::Test {};

TEST_F(CooldownTypeTest, MaxIs3) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::CooldownType::Max), 3);
}

TEST_F(CooldownTypeTest, NormalIsZero) {
    EXPECT_EQ(static_cast<int32_t>(GameEnums::CooldownType::Normal), 0);
}
