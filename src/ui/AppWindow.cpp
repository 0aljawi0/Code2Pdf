#include "ui/AppWindow.h"
#include "ui/Widgets.h"
#include "ui/Theme.h"
#include "core/PathUtils.h"

#include <algorithm>
#include <cstdio>

namespace code2pdf
{
	AppWindow::AppWindow(IDialogService& dialogs)
		: dialogs_(dialogs), successDialog_(dialogs)
	{}


    // DATA PIPELINE
	void AppWindow::RebuildIfNeeded() {
		if (!treeDirty_) return;
		treeDirty_ = false;
		roots_ = treeBuilder_.Build(files_);
	}

    SelectionCounts AppWindow::GetCounts(const TreeNode* n) {
        auto it = countsCache_.find(n);
        if (it != countsCache_.end()) return it->second;

        SelectionCounts c;
        if (!n->isFolder) {
            c.total = 1;
            c.selected = selected_.count(n->path) ? 1 : 0;
        }
        else {
            for (const auto& ch : n->children) {
                SelectionCounts cc = GetCounts(ch.get());
                c.total += cc.total;
                c.selected += cc.selected;
            }
        }
        countsCache_[n] = c;
        return c;
    }

    void AppWindow::ToggleFile(const std::string& path) {
        if (selected_.count(path))
        {
            selected_.erase(path);
        }
        else
        {
            selected_.insert(path);
        }
    }

    void AppWindow::ToggleFolder(const TreeNode* node) {
        std::vector<std::string> paths;
        std::vector<const TreeNode*> stack;

        stack.push_back(node);

        while (!stack.empty())
        {
            const TreeNode* n = stack.back();
            stack.pop_back();
            if (n->isFolder)
            {
                for (const auto& ch : n->children)
                {
                    stack.push_back(ch.get());
                }
            }
            else
            {
                paths.push_back(n->path);
            }
        }

        bool all = !paths.empty();
        for (const auto& p : paths)
        {
            if (!selected_.count(p))
            {
                all = false;
                break;
            }
        }

        if (all)
        {
            for (const auto& p : paths) selected_.erase(p);
        }
        else
        {
            for (const auto& p : paths) selected_.insert(p);
        }
    }

    // COMMANDS
    void AppWindow::AddFiles() {
        error_.clear();
        auto picked = dialogs_.PickFiles();
        if (picked.empty()) return;

        std::vector<FileEntry> entries;
        for (const auto& p : picked) {
            FileEntry fe;
            if (scanner_.GetFileEntry(p, fe)) entries.push_back(fe);
        }
        if (entries.empty()) return;

        std::set<std::string> seen;
        for (const auto& f : files_) seen.insert(f.path);
        for (auto& e : entries) {
            if (seen.count(e.path)) continue;
            files_.push_back(e);
            selected_.insert(e.path);
            seen.insert(e.path);
        }
        treeDirty_ = true;

        if (sourceName_.empty()) {
            sourceName_ = PathUtils::BaseName(entries.front().path);
            if (sourceName_.empty()) sourceName_ = "code2pdf";
            SetOutputPath(dialogs_.SuggestOutputPath(sourceName_));
        }
    }

    void AppWindow::AddFolder() {
        error_.clear();
        const std::string dir = dialogs_.PickFolder();
        if (dir.empty()) return;

        auto entries = scanner_.ScanFolder(dir, settings_);
        std::set<std::string> seen;
        for (const auto& f : files_) seen.insert(f.path);
        for (auto& e : entries) {
            if (seen.count(e.path)) continue;
            files_.push_back(e);
            selected_.insert(e.path);
            seen.insert(e.path);
        }
        treeDirty_ = true;

        sourceName_ = PathUtils::BaseName(dir);
        if (sourceName_.empty()) sourceName_ = "code2pdf";
        SetOutputPath(dialogs_.SuggestOutputPath(sourceName_));
    }

    void AppWindow::ClearAll() {
        files_.clear();
        selected_.clear();
        collapsed_.clear();
        sourceName_.clear();
        outputPathBuf_[0] = '\0';
        error_.clear();
        treeDirty_ = true;
    }

    void AppWindow::Export() {
        error_.clear();
        std::vector<std::string> toExport;
        for (const auto& f : files_)
            if (selected_.count(f.path)) toExport.push_back(f.path);

        if (toExport.empty()) {
            error_ = "Select at least one file to export.";
            return;
        }

        std::string target = PathUtils::Trim(OutputPath());
        if (target.empty())
            target = dialogs_.SuggestOutputPath(sourceName_.empty() ? "code2pdf" : sourceName_);
        const std::string lower = PathUtils::ToLower(target);
        if (lower.size() < 4 || lower.substr(lower.size() - 4) != ".pdf") target += ".pdf";
        SetOutputPath(target);

        busy_ = true;
        auto res = pdfWriter_.Generate(target, toExport);
        busy_ = false;

        if (res.ok) successDialog_.Open(target);
        else        error_ = res.error;
    }

    //UI
    void AppWindow::DrawToolbar() {
        if (ImGui::Button("Add files")) AddFiles();
        ImGui::SameLine();
        if (ImGui::Button("Add folder")) AddFolder();

        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled(
            "%d of %d file%s selected",
            (int)selected_.size(), 
            (int)files_.size(),
            files_.size() == 1 ? "" : "s"
        );

        ImGuiStyle& st = ImGui::GetStyle();
        const float sp = st.ItemSpacing.x;
        const float bw = ImGui::CalcTextSize("Settings").x + st.FramePadding.x * 2.0f;
        const float cw = ImGui::CalcTextSize("Clear").x + st.FramePadding.x * 2.0f;
        const float rightW = bw + cw + sp;

        ImGui::SameLine(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - rightW);

        if (ImGui::Button("Settings")) requestSettings_ = true;
        ImGui::SameLine();
        ImGui::BeginDisabled(files_.empty());
        if (ImGui::Button("Clear")) ClearAll();
        ImGui::EndDisabled();
    }

    void AppWindow::DrawRow(const TreeNode* n, int idx, float rowH) {
        ImGui::PushID(idx);
        const float rowW = ImGui::GetContentRegionAvail().x;
        const ImVec2 rowMin = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        if (n->depth & 1)
            dl->AddRectFilled(rowMin, ImVec2(rowMin.x + rowW, rowMin.y + rowH),
                ImGui::GetColorU32(ImGuiCol_FrameBg, 0.18f));

        // ---- allow the chevron / checkbox / icons to be hovered and clicked ----
        ImGui::SetNextItemAllowOverlap();
        ImGui::InvisibleButton("##row", ImVec2(rowW, rowH));
        if (ImGui::IsItemHovered())
            dl->AddRectFilled(rowMin, ImVec2(rowMin.x + rowW, rowMin.y + rowH),
                ImGui::GetColorU32(ImGuiCol_HeaderHovered, 0.35f));

        float x = rowMin.x + 4.0f + n->depth * 16.0f;
        float midY = rowMin.y + rowH * 0.5f;

        // chevron
        if (n->isFolder && !n->children.empty()) {
            const bool open = collapsed_.find(n->path) == collapsed_.end();
            ImGui::SetCursorScreenPos(ImVec2(x, midY - 8.0f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
            if (ImGui::ArrowButton("##chev", open ? ImGuiDir_Down : ImGuiDir_Right)) {
                if (open) collapsed_.insert(n->path);
                else      collapsed_.erase(n->path);
            }
            ImGui::PopStyleColor(2);
        }
        x += 20.0f;

        // checkbox
        bool checked = false, indeterminate = false;
        if (n->isFolder) {
            SelectionCounts c = GetCounts(n);
            checked = c.total > 0 && c.selected == c.total;
            indeterminate = c.selected > 0 && c.selected < c.total;
        }
        else {
            checked = selected_.count(n->path) > 0;
        }
        ImGui::SetCursorScreenPos(ImVec2(x, midY - 8.0f));
        if (Widgets::TriCheckbox(checked, indeterminate, 16.0f)) {
            if (n->isFolder) ToggleFolder(n);
            else             ToggleFile(n->path);
        }
        x += 22.0f;

        // icon + name
        Widgets::DrawNodeIcon(dl, ImVec2(x, midY - 7.0f), n->isFolder);
        x += 20.0f;

        const float nameRight = rowMin.x + rowW - 96.0f;
        dl->PushClipRect(ImVec2(x, rowMin.y), ImVec2(nameRight, rowMin.y + rowH), true);
        ImU32 nameCol = n->isFolder ? Theme::FolderText : ImGui::GetColorU32(ImGuiCol_Text);
        dl->AddText(ImVec2(x, midY - ImGui::GetTextLineHeight() * 0.5f), nameCol, n->name.c_str());
        dl->PopClipRect();

        if (!n->isFolder) {
            const std::string sz = PathUtils::FormatSize(n->size);
            ImVec2 ts = ImGui::CalcTextSize(sz.c_str());
            dl->AddText(ImVec2(rowMin.x + rowW - ts.x - 8.0f, midY - ts.y * 0.5f),
                ImGui::GetColorU32(ImGuiCol_TextDisabled), sz.c_str());
        }

        ImGui::SetCursorScreenPos(ImVec2(rowMin.x, rowMin.y + rowH));
        ImGui::PopID();
    }

    void AppWindow::DrawTree(ImVec2 size) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
        ImGui::BeginChild("treebox", size, ImGuiChildFlags_Borders);

        const float availW = ImGui::GetContentRegionAvail().x;
        ImGui::TextDisabled("Name");
        ImGui::SameLine(availW - ImGui::CalcTextSize("Size").x - 6.0f);
        ImGui::TextDisabled("Size");
        ImGui::Separator();

        ImGui::BeginChild("rows", ImVec2(0, 0));
        if (files_.empty()) {
            ImGui::TextDisabled("No files yet - add files or a folder to get started.");
        }
        else if (flat_.empty())
        {
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(Theme::Warning),
                "Loaded %zu files but tree is empty - check CommonRoot/Build.",
                files_.size());
        }
        else 
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const float rowH = ImGui::GetTextLineHeight() + 8.0f;
            for (int i = 0; i < (int)flat_.size(); i++) DrawRow(flat_[i], i, rowH);
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    void AppWindow::DrawFooter() {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Output");
        ImGui::SameLine();

        const float browseW = 90.0f, exportW = 110.0f;
        const float sp = ImGui::GetStyle().ItemSpacing.x;
        ImGui::SetNextItemWidth(-(browseW + exportW + sp * 2.0f));
        ImGui::InputTextWithHint("##output",
            "Auto-filled from the scanned folder - or click Browse",
            outputPathBuf_, sizeof(outputPathBuf_));
        ImGui::SameLine();
        if (ImGui::Button("Browse", ImVec2(browseW, 0))) {
            std::string p = dialogs_.PickSavePath(OutputPath());
            if (!p.empty()) SetOutputPath(p);
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(files_.empty() || busy_);
        if (ImGui::Button(busy_ ? "Exporting..." : "Export", ImVec2(exportW, 0))) Export();
        ImGui::EndDisabled();
    }

    void AppWindow::DrawError(float height) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(Theme::DangerBg));
        ImGui::PushStyleColor(ImGuiCol_Border, ImGui::ColorConvertU32ToFloat4(Theme::DangerBorder));
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(Theme::DangerText));
        ImGui::BeginChild("err", ImVec2(0, height), ImGuiChildFlags_Borders);
        ImGui::TextWrapped("%s", error_.c_str());
        ImGui::EndChild();
        ImGui::PopStyleColor(3);
    }

    void AppWindow::Draw() {
        RebuildIfNeeded();
        flat_.clear();
        TreeBuilder::Flatten(roots_, collapsed_, flat_);
        countsCache_.clear();

        const ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 12));
        ImGui::Begin("##code2pdf_root", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleVar(3);

        DrawToolbar();
        ImGui::Spacing();

        ImGuiStyle& st = ImGui::GetStyle();
        const float frameH = ImGui::GetFrameHeight();
        float       errH = 0;
        if (!error_.empty()) {
            float wrapW = ImGui::GetContentRegionAvail().x - st.WindowPadding.x * 2.0f - 8.0f;
            if (wrapW < 60.0f) wrapW = 60.0f;
            ImVec2 esz = ImGui::CalcTextSize(error_.c_str(), nullptr, false, wrapW);
            errH = esz.y + st.WindowPadding.y * 2.0f + 4.0f;
        }
        const float reserved = frameH + st.ItemSpacing.y * 2.0f +
            (error_.empty() ? 0.0f : errH + st.ItemSpacing.y);
        float treeH = ImGui::GetContentRegionAvail().y - reserved;
        if (treeH < 100.0f) treeH = 100.0f;

        DrawTree(ImVec2(0, treeH));
        ImGui::Spacing();
        DrawFooter();
        if (!error_.empty()) {
            ImGui::Spacing();
            DrawError(errH);
        }

        ImGui::End();

        // dialogs (must be drawn outside the root window)
        if (requestSettings_) {
            requestSettings_ = false;
            settingsDialog_.Open(settings_);
        }
        ScanSettings saved;
        if (settingsDialog_.Draw(saved)) settings_ = saved;
        successDialog_.Draw();
    }
}