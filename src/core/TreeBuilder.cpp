#include "core/TreeBuilder.h"
#include "core/PathUtils.h"

#include <algorithm>
#include <unordered_map>

namespace code2pdf
{
    std::vector<std::unique_ptr<TreeNode>> TreeBuilder::Build(const std::vector<FileEntry>& files) const
    {
        //return std::vector<std::unique_ptr<TreeNode>>(); 
        std::vector<std::unique_ptr<TreeNode>> roots;
        if (files.empty()) return roots;

        std::vector<std::string> paths;
        paths.reserve(files.size());
        for (const auto& f : files) paths.push_back(f.path);
        const std::string common = CommonRoot(paths);

        std::unordered_map<std::string, TreeNode*> folderMap;
        for (const auto& f : files) {
            std::string rel = PathUtils::ToPosix(f.path);
            if (rel.size() > common.size()) rel = rel.substr(common.size());
            else rel.clear();
            if (!rel.empty() && rel.front() == '/') rel.erase(rel.begin());
            auto parts = PathUtils::Split(rel, '/');

            std::string current = common;
            std::vector<std::unique_ptr<TreeNode>>* level = &roots;

            for (size_t i = 0; i < parts.size(); i++) {
                const bool last = (i + 1 == parts.size());
                current = current.empty() ? parts[i] : current + "/" + parts[i];

                if (last) {
                    auto node = std::make_unique<TreeNode>();
                    node->id = f.path;
                    node->name = parts[i];
                    node->path = f.path;
                    node->isFolder = false;
                    node->size = f.size;
                    node->depth = (int)i;
                    level->push_back(std::move(node));
                }
                else {
                    TreeNode* folder = nullptr;
                    auto it = folderMap.find(current);
                    if (it != folderMap.end()) {
                        folder = it->second;
                    }
                    else {
                        auto node = std::make_unique<TreeNode>();
                        node->id = current;
                        node->name = parts[i];
                        node->path = current;
                        node->isFolder = true;
                        node->depth = (int)i;
                        folder = node.get();
                        folderMap[current] = folder;
                        level->push_back(std::move(node));
                    }
                    level = &folder->children;
                }
            }
        }
        SortTree(roots);
        return roots;
    }

    void TreeBuilder::Flatten(const std::vector<std::unique_ptr<TreeNode>>& roots, const std::set<std::string>& collapsed, std::vector<const TreeNode*>& out)
    {
        for (const auto& n : roots) {
            out.push_back(n.get());
            if (n->isFolder && !collapsed.count(n->path)) Flatten(n->children, collapsed, out);
        }
    }

    std::string TreeBuilder::CommonRoot(const std::vector<std::string>& paths) {
        if (paths.empty()) return {};
        std::vector<std::vector<std::string>> split;
        split.reserve(paths.size());
        for (const auto& p : paths) split.push_back(PathUtils::Split(PathUtils::ToPosix(p), '/'));

        const auto& first = split.front();
        size_t common = 0;
        for (size_t i = 0; i < first.size(); i++) {
            bool all = true;
            for (const auto& s : split) {
                if (i >= s.size() || s[i] != first[i]) { all = false; break; }
            }
            if (all) common = i + 1; else break;
        }
        if (common == first.size()) common = first.size() >= 2 ? 1 : 0;

        std::string root;
        for (size_t i = 0; i < common; i++) {
            if (i) root += '/';
            root += first[i];
        }
        return root;
    }

    void TreeBuilder::SortTree(std::vector<std::unique_ptr<TreeNode>>& nodes) 
    {
        std::sort(nodes.begin(), nodes.end(),
            [](const std::unique_ptr<TreeNode>& a, const std::unique_ptr<TreeNode>& b) {
                if (a->isFolder != b->isFolder) return a->isFolder;
                return a->name < b->name;
            });
        for (auto& n : nodes) SortTree(n->children);
    }
}