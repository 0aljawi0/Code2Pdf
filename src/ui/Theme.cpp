#include "ui/Theme.h"
#include "imgui.h"

#include <filesystem>
#include <string>
#include <vector>

#include "fonts/OpenSansRegular.cpp"
#include "fonts/OpenSansLight.cpp"
#include "fonts/OpenSansBold.cpp"

namespace code2pdf
{
    namespace
    {
        // ---------------------------------------------------------------------------
        // Spectrum (dark) palette
        //
        // The light Spectrum style has been inverted: surfaces become dark grays and
        // text becomes light.  FrameBg is lifted to GRAY200 (rather than the literal
        // inverse of GRAY75) so form controls stay a touch lighter than the window
        // background — otherwise the whole UI reads as "holes" in the window.
        // ---------------------------------------------------------------------------
        constexpr ImU32 GRAY50 = IM_COL32(8, 8, 8, 255);
        constexpr ImU32 GRAY75 = IM_COL32(26, 26, 26, 255);
        constexpr ImU32 GRAY100 = IM_COL32(34, 34, 34, 255);
        constexpr ImU32 GRAY200 = IM_COL32(44, 44, 44, 255);
        constexpr ImU32 GRAY300 = IM_COL32(58, 58, 58, 255);
        constexpr ImU32 GRAY400 = IM_COL32(85, 85, 85, 255);
        constexpr ImU32 GRAY500 = IM_COL32(122, 122, 122, 255);
        constexpr ImU32 GRAY600 = IM_COL32(160, 160, 160, 255);
        constexpr ImU32 GRAY700 = IM_COL32(200, 200, 200, 255);
        constexpr ImU32 GRAY800 = IM_COL32(227, 227, 227, 255);
        constexpr ImU32 GRAY900 = IM_COL32(255, 255, 255, 255);

        constexpr ImU32 BLUE400 = IM_COL32(55, 142, 240, 255);
        constexpr ImU32 BLUE500 = IM_COL32(38, 128, 235, 255);
        constexpr ImU32 BLUE600 = IM_COL32(20, 115, 230, 255);

        inline ImVec4 F4(ImU32 c) { return ImGui::ColorConvertU32ToFloat4(c); }
        inline ImVec4 F4A(ImU32 c, float a) { ImVec4 v = F4(c); v.w = a; return v; }

        std::filesystem::path GetProjectRoot()
        {
            std::filesystem::path p(__FILE__);
            if (p.is_relative())
                p = std::filesystem::absolute(p);
            return p.parent_path().parent_path().parent_path();
        }
    }

	void Theme::Apply()
	{
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();

        // Load Regular (16px)
        io.Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansRegular_compressed_data_base85, 16.0f);

        // Load Light (16px and 32px)
        io.Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansLight_compressed_data_base85, 16.0f);
        io.Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansLight_compressed_data_base85, 32.0f);

        // Load Regular (11px) and Bold (11px)
        io.Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansRegular_compressed_data_base85, 11.0f);
        io.Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansBold_compressed_data_base85, 11.0f);

        // --- style metrics -----------------------------------------------------
        ImGuiStyle& style = ImGui::GetStyle();
        style = ImGuiStyle();

        style.Alpha = 1.0f;
        style.DisabledAlpha = 0.45f;
        style.WindowPadding = ImVec2(15, 15);
        style.WindowRounding = 5.0f;
        style.FramePadding = ImVec2(5, 5);
        style.FrameRounding = 4.0f;
        style.ItemSpacing = ImVec2(12, 8);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 15.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 5.0f;
        style.GrabRounding = 4.0f;
        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);

        // --- Spectrum dark colors ---------------------------------------------
        ImVec4* c = style.Colors;

        c[ImGuiCol_Text] = F4(GRAY800);
        c[ImGuiCol_TextDisabled] = F4(GRAY500);
        c[ImGuiCol_WindowBg] = F4(GRAY100);
        c[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_PopupBg] = F4(GRAY75);
        c[ImGuiCol_Border] = F4(GRAY300);
        c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

        c[ImGuiCol_FrameBg] = F4(GRAY200);
        c[ImGuiCol_FrameBgHovered] = F4(GRAY300);
        c[ImGuiCol_FrameBgActive] = F4(GRAY400);

        c[ImGuiCol_TitleBg] = F4(GRAY75);
        c[ImGuiCol_TitleBgActive] = F4(GRAY100);
        c[ImGuiCol_TitleBgCollapsed] = F4A(GRAY75, 0.75f);
        c[ImGuiCol_MenuBarBg] = F4(GRAY100);

        c[ImGuiCol_ScrollbarBg] = F4(GRAY100);
        c[ImGuiCol_ScrollbarGrab] = F4(GRAY400);
        c[ImGuiCol_ScrollbarGrabHovered] = F4(GRAY500);
        c[ImGuiCol_ScrollbarGrabActive] = F4(GRAY600);

        c[ImGuiCol_CheckMark] = F4(BLUE400);
        c[ImGuiCol_SliderGrab] = F4(GRAY600);
        c[ImGuiCol_SliderGrabActive] = F4(GRAY700);

        c[ImGuiCol_Button] = F4(GRAY200);
        c[ImGuiCol_ButtonHovered] = F4(GRAY300);
        c[ImGuiCol_ButtonActive] = F4(GRAY400);

        c[ImGuiCol_Header] = F4A(BLUE400, 0.40f);
        c[ImGuiCol_HeaderHovered] = F4A(BLUE400, 0.60f);
        c[ImGuiCol_HeaderActive] = F4A(BLUE500, 0.80f);

        c[ImGuiCol_Separator] = F4(GRAY300);
        c[ImGuiCol_SeparatorHovered] = F4(GRAY400);
        c[ImGuiCol_SeparatorActive] = F4(GRAY500);

        c[ImGuiCol_ResizeGrip] = F4A(GRAY400, 0.50f);
        c[ImGuiCol_ResizeGripHovered] = F4A(GRAY500, 0.80f);
        c[ImGuiCol_ResizeGripActive] = F4A(GRAY600, 0.90f);

        c[ImGuiCol_PlotLines] = F4(BLUE400);
        c[ImGuiCol_PlotLinesHovered] = F4(BLUE600);
        c[ImGuiCol_PlotHistogram] = F4(BLUE400);
        c[ImGuiCol_PlotHistogramHovered] = F4(BLUE600);

        c[ImGuiCol_TextSelectedBg] = F4A(BLUE400, 0.35f);
        c[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        c[ImGuiCol_NavHighlight] = F4A(GRAY900, 0.06f);
        c[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        c[ImGuiCol_NavWindowingDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);
        c[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.55f);

        // Tabs ------------------------------------------------------------------
        c[ImGuiCol_Tab] = F4(GRAY200);
        c[ImGuiCol_TabHovered] = F4A(BLUE400, 0.60f);
        c[ImGuiCol_TabSelected] = F4(GRAY100);
        c[ImGuiCol_TabDimmed] = F4(GRAY75);
        c[ImGuiCol_TabDimmedSelected] = F4(GRAY200);
        /*c[ImGuiCol_TabHeaderBg] = F4(GRAY200);
        c[ImGuiCol_TabBorderStrong] = F4(GRAY300);
        c[ImGuiCol_TabBorderLight] = F4(GRAY200);
        c[ImGuiCol_TabRowBg] = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_TabRowBgAlt] = F4A(GRAY900, 0.03f);*/
	}
}