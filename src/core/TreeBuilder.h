#pragma once
#include "core/Models.h"
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace code2pdf {

    class TreeBuilder {
    public:
        std::vector<std::unique_ptr<TreeNode>> Build(const std::vector<FileEntry>& files) const;

        static void Flatten(
            const std::vector<std::unique_ptr<TreeNode>>& roots,
            const std::set<std::string>& collapsed,
            std::vector<const TreeNode*>& out
        );

    private:
        static std::string CommonRoot(const std::vector<std::string>& paths);
        static void SortTree(std::vector<std::unique_ptr<TreeNode>>& nodes);
    };

}