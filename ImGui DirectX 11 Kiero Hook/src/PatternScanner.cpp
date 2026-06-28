#include "../include/PatternScanner.h"
#include <psapi.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>

namespace PatternScanner {
    ScanResult ScanPattern(HMODULE module, const std::string& pattern, const std::string& mask) {
        if (!module) {
            std::cerr << "[PatternScanner] ScanPattern called with null module" << std::endl;
            return { 0, false };
        }

        MODULEINFO modInfo;
        if (!GetModuleInformation(GetCurrentProcess(), module, &modInfo, sizeof(modInfo))) {
            std::cerr << "[PatternScanner] GetModuleInformation failed (error " << GetLastError() << ")" << std::endl;
            return { 0, false };
        }

        return ScanPattern(reinterpret_cast<uintptr_t>(module), modInfo.SizeOfImage, pattern, mask);
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

    HMODULE GetModule(const std::string& moduleName) {
        return GetModuleHandleA(moduleName.c_str());
    }

    size_t GetModuleSize(HMODULE module) {
        if (!module) {
            std::cerr << "[PatternScanner] GetModuleSize called with null module" << std::endl;
            return 0;
        }

        MODULEINFO modInfo;
        if (GetModuleInformation(GetCurrentProcess(), module, &modInfo, sizeof(modInfo))) {
            return modInfo.SizeOfImage;
        }

        std::cerr << "[PatternScanner] GetModuleInformation failed (error " << GetLastError() << ")" << std::endl;
        return 0;
    }

    std::vector<uint8_t> PatternToBytes(const std::string& pattern) {
        std::vector<uint8_t> bytes;
        std::string current;

        auto parseToken = [&](const std::string& token) -> bool {
            try {
                bytes.push_back(static_cast<uint8_t>(std::stoi(token, nullptr, 16)));
                return true;
            } catch (const std::invalid_argument&) {
                std::cerr << "[PatternScanner] Invalid hex token in pattern: '" << token << "'" << std::endl;
                return false;
            } catch (const std::out_of_range&) {
                std::cerr << "[PatternScanner] Hex token out of range: '" << token << "'" << std::endl;
                return false;
            }
        };

        for (char c : pattern) {
            if (c == ' ') {
                if (!current.empty()) {
                    if (!parseToken(current)) return {};
                    current.clear();
                }
            } else {
                current += c;
            }
        }

        if (!current.empty()) {
            if (!parseToken(current)) return {};
        }

        return bytes;
    }
}
