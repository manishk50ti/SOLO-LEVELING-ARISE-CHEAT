#pragma once
#include <cstdint>

// Game Enums from Solo Leveling: Arise dump.cs

namespace GameEnums {
    // EGuildStatType (dump.cs:236826) - Combat stats
    enum class GuildStatType : int32_t {
        AddAtt = 0,              // +Attack
        AddArm = 1,              // +Armor
        AddMHP = 2,              // +Max HP
        AAddMMP = 3,             // +Max MP
        Critical = 4,            // Crit rate
        CriDam = 5,              // Crit damage
        Precision = 6,
        ArmPen = 7,              // Armor penetration
        Dam = 8,                 // +Damage
        EnhanceSkillTypeDamage_BaseAttack = 9,
        EnhanceSkillTypeDamage_CoreAttack = 10,
        EnhanceSkillTypeDamage_NormalSkill = 11,
        EnhanceSkillTypeDamage_QTE = 12,
        EnhanceSkillTypeDamage_Supporter = 13,
        EnhanceSkillTypeDamage_UltimateSkill = 14,
        ElementDamFire = 15,
        ElementDamWater = 16
    };

    // EClassType (dump.cs:231791) - Hunter classes
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

    // ERankType (dump.cs:231873) - Hunter ranks
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

    // ECharacterType (dump.cs:231791)
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

    // EFactionType (dump.cs:231862)
    enum class FactionType : int32_t {
        PlayerSide = 0,
        MonsterSide = 1,
        Neutral = 3
    };

    // EBattleContentType (dump.cs:231424) - Battle modes
    enum class BattleContentType : int32_t {
        None = 0,
        MainStory = 1,
        SubStory = 2,
        NormalGate = 3,
        RedGate = 4,
        DungeonBreak = 5,
        EventDungeon = 6,
        InstantDungeon = 7,
        TimeAttack = 8,
        WorldBoss = 9,
        Tower = 10,
        OtherWorld = 11,
        Raid = 12,
        Advent = 13,
        TimeAttackRelay = 14,
        SpecialGate = 15,
        HunterStory = 16,
        HunterExperience = 17,
        ChaosBattleField = 18,
        ChaosBattleFieldEvent = 19,
        JobChange = 20,
        GuildBoss = 21,
        EventMiniDungeon = 22,
        OmnibusStoryEvent = 23,
        ChampionshipPre = 24,
        DimensionGate = 25,
        EventCoopDungeon = 26,
        HPSynchBoss = 27,
        AsyncPvp = 28,
        ArchitectTrial = 29,
        LibraryDungeon = 30,
        MultiBoss = 31,
        TrainingRoom = 32,
        CoopDungeon = 33,
        NewbieProtocol = 34,
        ArtifactDungeon = 35,
        CoreDungeon = 36
    };

    // EWeaponCategory (dump.cs:237177)
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

    // EItemRarity (dump.cs:232986)
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

    // EBuffLargeType (dump.cs:230860)
    enum class BuffLargeType : int32_t {
        None = 0,
        Buff = 1,
        DeBuff = 2,
        Passive = 3,
        Special = 4,
        ShadowOnly = 5
    };

    // ECooldownType (dump.cs:230889)
    enum class CooldownType : int32_t {
        Normal = 0,
        AfterCool = 1,
        AfterBuffCool = 2,
        Max = 3
    };

    // EGuildType (dump.cs:234517) - In-universe guilds
    enum class GuildType : int32_t {
        None = 0,
        Ahjin = 1,              // Sung Jinwoo's guild
        Hunters = 2,
        WhiteTiger = 3,
        Fiend = 4,
        Fame = 5,
        Knights = 6,
        DrawSword = 7,
        Scavenger = 8,
        HunterAssociation = 9,
        Justitia = 10,
        USAHunter = 11,
        JPHunter = 12,
        Helper = 13,
        Valkyrie = 14,
        Richter = 15,
        idle = 16,
        Unknown = 17,
        AdventureCompanions = 18,
        Daybreak = 19
    };

    // EMonarchType (dump.cs:231905) - The Monarchs
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

    // EUnlockType (dump.cs:234952)
    enum class UnlockType : int32_t {
        None = 0,
        DefaultOpen = 1,
        StoryChapter = 2,
        StoryMission = 3,
        LevelSJW = 4,
        LevelAccount = 5,
        TutorialEnd = 6,
        HunterLevel = 7,
        BattleClass = 8,
        ChangeJobSJW = 9,
        PackagePurchase = 10,
        UseItem = 11,
        GetItem = 12,
        GemSlotID = 13,
        BattleMission = 14,
        LevelGuild = 15,
        RaidDungeonClear = 16,
        PackageMaximumPurchase = 17,
        QuestTaskComplete = 18,
        HaveItemGemGrade = 19,
        Subscribe = 20,
        GetShadow_Target = 21,
        ContentsUnlock = 22
    };
}
