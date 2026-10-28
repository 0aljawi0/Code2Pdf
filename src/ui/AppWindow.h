#pragma once
#include "core/Models.h"
#include "core/Scanner.h"
#include "core/TreeBuilder.h"
#include "core/PdfWriter.h"
#include "platform/IDialogService.h"
#include "ui/SettingsDialog.h"
#include "ui/SuccessDialog.h"

#include "imgui.h"

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace code2pdf
{
	class AppWindow
	{
    public:
        explicit AppWindow(IDialogService& dialogs);

        void Draw();

    private:
        // data pipeline
        void RebuildIfNeeded();
        SelectionCounts GetCounts(const TreeNode* node);
        void ToggleFile(const std::string& path);
        void ToggleFolder(const TreeNode* node);

        // commands
        void AddFiles();
        void AddFolder();
        void ClearAll();
        void Export();

        // ui parts
        void DrawToolbar();
        void DrawTree(ImVec2 size);
        void DrawRow(const TreeNode* node, int idx, float rowH);
        void DrawFooter();
        void DrawError(float height);

        // services
        IDialogService& dialogs_;
        Scanner scanner_;
        TreeBuilder treeBuilder_;
        PdfWriter pdfWriter_;
        SettingsDialog settingsDialog_;
        SuccessDialog successDialog_;

        // state
        std::vector<FileEntry> files_;
        std::set<std::string> selected_;
        std::set<std::string> collapsed_;
        std::string sourceName_;
        char outputPathBuf_[1024] = "";
        std::string error_;
        bool busy_ = false;
        bool requestSettings_ = false;

        ScanSettings settings_;

        // derived tree state
        bool treeDirty_ = true;
        std::vector<std::unique_ptr<TreeNode>> roots_;
        std::vector<const TreeNode*> flat_;
        std::unordered_map<const TreeNode*, SelectionCounts> countsCache_;

        std::string OutputPath() const { return outputPathBuf_; }
        void SetOutputPath(const std::string& p) {
            snprintf(outputPathBuf_, sizeof(outputPathBuf_), "%s", p.c_str());
        }
	};
}