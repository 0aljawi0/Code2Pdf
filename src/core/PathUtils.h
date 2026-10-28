#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace code2pdf {

    class PathUtils {
    public:
        static std::string ToLower(std::string s);
        static std::string Trim(const std::string& s);
        static std::string ToPosix(std::string p);
        static std::string BaseName(const std::string& p);
        static std::string FormatSize(uint64_t bytes);
        static std::vector<std::string> Split(const std::string& s, char sep);
    };

}