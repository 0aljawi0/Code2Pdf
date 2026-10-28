#pragma once
#include "core/Models.h"
#include <string>
#include <vector>

namespace code2pdf 
{

    class Scanner {
    public:
        std::vector<FileEntry> ScanFolder(const std::string& root, const ScanSettings& s) const;
        bool GetFileEntry(const std::string& path, FileEntry& out) const;

    private:
        std::vector<std::string> BuildSkipList(const ScanSettings& s) const;
        std::vector<std::string> BuildExtList(const ScanSettings& s) const;
        static bool ShouldSkipDir(
            const std::string& lowerName,
            const std::vector<std::string>& skip
        );
    };

}