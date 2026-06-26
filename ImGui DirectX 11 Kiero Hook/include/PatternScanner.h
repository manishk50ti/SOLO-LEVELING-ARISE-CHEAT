#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <cstdint>

namespace PatternScanner {
    // Pattern scan result
    struct ScanResult {
        uintptr_t address;
        bool found;
    };

    // Scan for a byte pattern in a module
    ScanResult ScanPattern(HMODULE module, const std::string& pattern, const std::string& mask);

    // Scan for a byte pattern in a memory range
    ScanResult ScanPattern(uintptr_t start, size_t size, const std::string& pattern, const std::string& mask);

    // Get module base address
    HMODULE GetModule(const std::string& moduleName);

    // Get module size
    size_t GetModuleSize(HMODULE module);

    // Pattern to byte array conversion
    std::vector<uint8_t> PatternToBytes(const std::string& pattern);

    // Common patterns for Solo Leveling: Arise
    namespace Patterns {
        // Pattern for damage calculation function
        constexpr const char* DamageCalcPattern = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B DA";
        constexpr const char* DamageCalcMask = "xxxx?xxxx?xxxxxxx";

        // Pattern for stat getter
        constexpr const char* StatGetterPattern = "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56";
        constexpr const char* StatGetterMask = "xxxxxxxxxxxxxxxxxxxxxx";

        // Pattern for cooldown function
        constexpr const char* CooldownPattern = "40 53 48 83 EC 20 48 8B D9 48 85 C9 74 ? 48 8B 01";
        constexpr const char* CooldownMask = "xxxxxxxxxxxxx?xxx";

        // Pattern for HP setter
        constexpr const char* HPSetterPattern = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30";
        constexpr const char* HPSetterMask = "xxxx?xxxx?xxxx?xxxxx";
    }
}
