#include "ui/SettingsDialog.h"
#include "ui/Widgets.h"
#include "ui/Theme.h"
#include "core/Recommends.h"
#include "imgui.h"

namespace code2pdf
{
	namespace {
		constexpr const char* kTitle = "Code2Pdf settings";
	}


	void SettingsDialog::Open(const ScanSettings& current)
	{
		draft_ = current;
		folderDraft_[0] = 0;
		extDraft_[0] = 0;
		pendingOpen_ = true;
	}

	bool SettingsDialog::Draw(ScanSettings& savedOut)
	{
        if (pendingOpen_) {
            pendingOpen_ = false;
            ImGui::OpenPopup(kTitle);
        }

        bool saved = false;
        ImGui::SetNextWindowSize(ImVec2(760, 680), ImGuiCond_Appearing);
        if (!ImGui::BeginPopupModal(kTitle, nullptr, ImGuiWindowFlags_NoSavedSettings))
            return false;

        ImGui::TextWrapped("Applied to folder scans. Existing file queue is not affected.");
        ImGui::Spacing();

        const float footerH = ImGui::GetFrameHeightWithSpacing() +
            ImGui::GetStyle().ItemSpacing.y * 2.0f;

        ImGui::BeginChild("settingsScroll", ImVec2(0, -footerH));

        // ----- Excluded folders -----
        {
            ImGui::TextUnformatted("Excluded folders");
            const float sw = ImGui::GetFrameHeight() * 1.9f;
            ImGui::SameLine(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - sw);
            Widgets::ToggleSwitch("recFolders", &draft_.useRecommendedFolders);
            ImGui::TextDisabled("Skipped during directory traversal.");

            if (draft_.useRecommendedFolders) {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(Theme::ChipBg));
                ImGui::BeginChild("recF", ImVec2(0, 92), ImGuiChildFlags_Borders);
                Widgets::ChipFlowStatic(Recommends::Folders(), ImGui::GetContentRegionAvail().x);
                ImGui::EndChild();
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();
            }

            Widgets::ChipInputBox("extraFolders", draft_.extraExcludedFolders,
                folderDraft_, sizeof(folderDraft_),
                "Add folder name - e.g. coverage, .terraform", false,
                "Press Enter or comma to add. Wildcards like *.egg-info are supported.");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ----- Included file types -----
        {
            ImGui::TextUnformatted("Included file types");
            const float sw = ImGui::GetFrameHeight() * 1.9f;
            ImGui::SameLine(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - sw);
            Widgets::ToggleSwitch("recExts", &draft_.useRecommendedExtensions);
            ImGui::TextDisabled("Only files with these extensions will be queued.");

            if (draft_.useRecommendedExtensions) {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(Theme::ChipBg));
                ImGui::BeginChild("recE", ImVec2(0, 140), ImGuiChildFlags_Borders);
                Widgets::ChipFlowStatic(Recommends::Extensions(), ImGui::GetContentRegionAvail().x);
                ImGui::EndChild();
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();
            }

            Widgets::ChipInputBox("extraExts", draft_.extraIncludedExtensions,
                extDraft_, sizeof(extDraft_),
                "Add extension - e.g. ts, tsx, py", true,
                "Case-insensitive. Leading dots are stripped automatically.");
        }

        ImGui::EndChild();
        ImGui::Separator();

        if (ImGui::Button("Reset", ImVec2(90, 0))) draft_ = ScanSettings{};

        const float bw = 90.0f;
        const float sp = ImGui::GetStyle().ItemSpacing.x;
        ImGui::SameLine(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (bw * 2 + sp));
        if (ImGui::Button("Cancel", ImVec2(bw, 0))) ImGui::CloseCurrentPopup();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        if (ImGui::Button("Save", ImVec2(bw, 0))) {
            savedOut = draft_;
            saved = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor();

        ImGui::EndPopup();
        return saved;
	}

}