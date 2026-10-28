#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace code2pdf
{
    struct FileEntry 
    {
        std::string path;
        std::string name;
        uint64_t    size = 0;
    };

    struct ScanSettings 
    {
        bool useRecommendedFolders = true;
        bool useRecommendedExtensions = true;
        std::vector<std::string> extraExcludedFolders;
        std::vector<std::string> extraIncludedExtensions;
    };

    struct TreeNode 
    {
        std::string id;
        std::string name;
        std::string path;
        bool        isFolder = false;
        uint64_t    size = 0;
        int         depth = 0;
        std::vector<std::unique_ptr<TreeNode>> children;
    };

    struct SelectionCounts 
    {
        int total = 0;
        int selected = 0;
    };
}