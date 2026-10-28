#include "ui/SuccessDialog.h"
#include "imgui.h"
#include "AppWindow.h"

namespace code2pdf
{
	namespace 
	{ 
		constexpr const char* kTitle = "Export complete"; 
	}


	void SuccessDialog::Open(const std::string& outputPath)
	{
		outputPath_ = outputPath;
		pendingOpen_ = true;
	}

    void SuccessDialog::Draw()
	{
        if (pendingOpen_) {
            pendingOpen_ = false;
            ImGui::OpenPopup(kTitle);
        }

        ImGui::SetNextWindowSize(ImVec2(480, 0), ImGuiCond_Appearing);
        if (!ImGui::BeginPopupModal(kTitle, nullptr, ImGuiWindowFlags_NoSavedSettings))
            return;

        ImGui::TextDisabled("PDF written to:");
        ImGui::TextWrapped("%s", outputPath_.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Close", ImVec2(100, 0))) ImGui::CloseCurrentPopup();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        if (ImGui::Button("Go to folder", ImVec2(130, 0))) {
            dialogs_.RevealInExplorer(outputPath_);
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor();

        ImGui::EndPopup();
	}

}