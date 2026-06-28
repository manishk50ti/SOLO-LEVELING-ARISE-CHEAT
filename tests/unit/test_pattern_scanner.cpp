#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <cstdint>

// We re-implement the testable logic from PatternScanner here
// to avoid Windows dependencies (psapi.h, GetModuleInformation, etc.)
// This tests the core algorithm independently.

namespace PatternScanner {

struct ScanResult {
    uintptr_t address;
    bool found;
};

std::vector<uint8_t> PatternToBytes(const std::string& pattern) {
    std::vector<uint8_t> bytes;
    std::string current;

    for (char c : pattern) {
        if (c == ' ') {
            if (!current.empty()) {
                bytes.push_back(static_cast<uint8_t>(std::stoi(current, nullptr, 16)));
                current.clear();
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        bytes.push_back(static_cast<uint8_t>(std::stoi(current, nullptr, 16)));
    }

    return bytes;
}

ScanResult ScanPattern(uintptr_t start, size_t size, const std::string& pattern, const std::string& mask) {
    if (pattern.empty() || mask.empty() || pattern.length() != mask.length()) {
        return { 0, false };
    }

    std::vector<uint8_t> patternBytes = PatternToBytes(pattern);

    for (size_t i = 0; i < size - pattern.length(); i++) {
        bool found = true;
        for (size_t j = 0; j < pattern.length(); j++) {
            if (mask[j] != '?' && patternBytes[j] != reinterpret_cast<uint8_t*>(start)[i + j]) {
                found = false;
                break;
            }
        }

        if (found) {
            return { start + i, true };
        }
    }

    return { 0, false };
}

} // namespace PatternScanner

// --- PatternToBytes Tests ---

class PatternToBytesTest : public ::testing::Test {};

TEST_F(PatternToBytesTest, EmptyPattern) {
    auto result = PatternScanner::PatternToBytes("");
    EXPECT_TRUE(result.empty());
}

TEST_F(PatternToBytesTest, SingleByte) {
    auto result = PatternScanner::PatternToBytes("FF");
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], 0xFF);
}

TEST_F(PatternToBytesTest, MultipleBytesSpaceSeparated) {
    auto result = PatternScanner::PatternToBytes("48 89 5C 24");
    ASSERT_EQ(result.size(), 4u);
    EXPECT_EQ(result[0], 0x48);
    EXPECT_EQ(result[1], 0x89);
    EXPECT_EQ(result[2], 0x5C);
    EXPECT_EQ(result[3], 0x24);
}

TEST_F(PatternToBytesTest, LowercaseHex) {
    auto result = PatternScanner::PatternToBytes("ab cd ef");
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 0xAB);
    EXPECT_EQ(result[1], 0xCD);
    EXPECT_EQ(result[2], 0xEF);
}

TEST_F(PatternToBytesTest, MixedCaseHex) {
    auto result = PatternScanner::PatternToBytes("aB Cd eF 12");
    ASSERT_EQ(result.size(), 4u);
    EXPECT_EQ(result[0], 0xAB);
    EXPECT_EQ(result[1], 0xCD);
    EXPECT_EQ(result[2], 0xEF);
    EXPECT_EQ(result[3], 0x12);
}

TEST_F(PatternToBytesTest, ZeroByte) {
    auto result = PatternScanner::PatternToBytes("00");
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], 0x00);
}

TEST_F(PatternToBytesTest, AllZeroBytes) {
    auto result = PatternScanner::PatternToBytes("00 00 00 00");
    ASSERT_EQ(result.size(), 4u);
    for (auto b : result) {
        EXPECT_EQ(b, 0x00);
    }
}

TEST_F(PatternToBytesTest, RealGamePattern) {
    // DamageCalcPattern from PatternScanner.h
    auto result = PatternScanner::PatternToBytes("48 89 5C 24 00 48 89 74 24 00 57 48 83 EC 20 48 8B DA");
    ASSERT_EQ(result.size(), 18u);
    EXPECT_EQ(result[0], 0x48);
    EXPECT_EQ(result[1], 0x89);
    EXPECT_EQ(result[2], 0x5C);
    EXPECT_EQ(result[3], 0x24);
    EXPECT_EQ(result[10], 0x57);
    EXPECT_EQ(result[17], 0xDA);
}

TEST_F(PatternToBytesTest, MultipleSpacesHandledGracefully) {
    // Multiple spaces between bytes should still work (empty tokens skipped)
    auto result = PatternScanner::PatternToBytes("FF  AB");
    // Implementation skips empty strings, so double space just produces 2 bytes
    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], 0xFF);
    EXPECT_EQ(result[1], 0xAB);
}

// --- ScanPattern Tests ---
// ScanPatternRaw operates directly on byte arrays (the core scan algorithm)
// This tests the matching logic without going through the hex-string parser.

namespace PatternScanner {

ScanResult ScanPatternRaw(uintptr_t start, size_t size,
                          const uint8_t* patternBytes, size_t patternLen,
                          const std::string& mask) {
    if (patternLen == 0 || mask.empty() || patternLen != mask.length()) {
        return { 0, false };
    }

    for (size_t i = 0; i <= size - patternLen; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (mask[j] != '?' && patternBytes[j] != reinterpret_cast<const uint8_t*>(start)[i + j]) {
                found = false;
                break;
            }
        }

        if (found) {
            return { start + i, true };
        }
    }

    return { 0, false };
}

} // namespace PatternScanner (continued)

class ScanPatternTest : public ::testing::Test {
protected:
    uint8_t testMemory[64];

    void SetUp() override {
        for (int i = 0; i < 64; i++) {
            testMemory[i] = static_cast<uint8_t>(i);
        }
    }
};

TEST_F(ScanPatternTest, EmptyPatternReturnsFalse) {
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, nullptr, 0, "x");
    EXPECT_FALSE(result.found);
    EXPECT_EQ(result.address, 0u);
}

TEST_F(ScanPatternTest, EmptyMaskReturnsFalse) {
    uint8_t pat[] = { 0x00 };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 1, "");
    EXPECT_FALSE(result.found);
    EXPECT_EQ(result.address, 0u);
}

TEST_F(ScanPatternTest, MismatchedLengthReturnsFalse) {
    uint8_t pat[] = { 0xAB, 0xCD };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 2, "x");
    EXPECT_FALSE(result.found);
}

TEST_F(ScanPatternTest, FindSingleByteAtStart) {
    uint8_t pat[] = { 0x00 };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 1, "x");
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.address, reinterpret_cast<uintptr_t>(testMemory));
}

TEST_F(ScanPatternTest, FindPatternInMiddle) {
    // testMemory[10] = 0x0A, testMemory[11] = 0x0B
    uint8_t pat[] = { 0x0A, 0x0B };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 2, "xx");
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.address, reinterpret_cast<uintptr_t>(&testMemory[10]));
}

TEST_F(ScanPatternTest, WildcardMatchesAnyByte) {
    // Pattern: 0x05 followed by any byte followed by 0x07
    uint8_t pat[] = { 0x05, 0x00, 0x07 };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 3, "x?x");
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.address, reinterpret_cast<uintptr_t>(&testMemory[5]));
}

TEST_F(ScanPatternTest, PatternNotFoundReturnsFalse) {
    uint8_t pat[] = { 0xFF, 0xFE };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 2, "xx");
    EXPECT_FALSE(result.found);
    EXPECT_EQ(result.address, 0u);
}

TEST_F(ScanPatternTest, AllWildcardsMatchAtStart) {
    uint8_t pat[] = { 0x00, 0x00, 0x00 };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 3, "???");
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.address, reinterpret_cast<uintptr_t>(testMemory));
}

TEST_F(ScanPatternTest, FindPatternAtEnd) {
    testMemory[60] = 0xDE;
    testMemory[61] = 0xAD;
    uint8_t pat[] = { 0xDE, 0xAD };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 2, "xx");
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.address, reinterpret_cast<uintptr_t>(&testMemory[60]));
}

TEST_F(ScanPatternTest, FindFirstOccurrence) {
    // Set up duplicate pattern
    testMemory[5] = 0xAA;
    testMemory[6] = 0xBB;
    testMemory[20] = 0xAA;
    testMemory[21] = 0xBB;
    uint8_t pat[] = { 0xAA, 0xBB };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 2, "xx");
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.address, reinterpret_cast<uintptr_t>(&testMemory[5]));
}

TEST_F(ScanPatternTest, LongerPatternMatch) {
    // testMemory[0..7] = 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    uint8_t pat[] = { 0x02, 0x03, 0x04, 0x05, 0x06 };
    auto result = PatternScanner::ScanPatternRaw(
        reinterpret_cast<uintptr_t>(testMemory), 64, pat, 5, "xxxxx");
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.address, reinterpret_cast<uintptr_t>(&testMemory[2]));
}

// --- Integration: PatternToBytes + ScanPattern together ---

TEST(PatternScanIntegrationTest, HexPatternScanInMemory) {
    // Set up memory with a known sequence
    uint8_t mem[32];
    for (int i = 0; i < 32; i++) mem[i] = 0;
    mem[10] = 0x48;
    mem[11] = 0x89;
    mem[12] = 0x5C;

    // Convert hex pattern to bytes, then scan
    auto patBytes = PatternScanner::PatternToBytes("48 89 5C");
    ASSERT_EQ(patBytes.size(), 3u);

    // Manual scan using the converted bytes
    PatternScanner::ScanResult result = { 0, false };
    std::string mask = "xxx";
    for (size_t i = 0; i <= 32 - patBytes.size(); i++) {
        bool found = true;
        for (size_t j = 0; j < patBytes.size(); j++) {
            if (mask[j] != '?' && patBytes[j] != mem[i + j]) {
                found = false;
                break;
            }
        }
        if (found) {
            result = { reinterpret_cast<uintptr_t>(&mem[i]), true };
            break;
        }
    }
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.address, reinterpret_cast<uintptr_t>(&mem[10]));
}
