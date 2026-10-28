#include "ui/Widgets.h"
#include "ui/Theme.h"
#include "core/PathUtils.h"

#include <algorithm>
#include <cctype>
#include <cmath>

namespace code2pdf
{
    namespace 
    {

        void CommitValue(std::vector<std::string>& values, std::string s, bool isExtension) 
        {
            s = PathUtils::Trim(s);
            if (isExtension) {
                while (!s.empty() && s.front() == '.') s.erase(s.begin());
                for (auto& c : s) c = (char)std::tolower((unsigned char)c);
            }
            if (s.empty()) return;
            const std::string lower = PathUtils::ToLower(s);
            for (const auto& v : values)
                if (PathUtils::ToLower(v) == lower) return;
            values.push_back(s);
        }

    }

    bool Widgets::TriCheckbox(bool checked, bool indeterminate, float size)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        bool clicked = ImGui::InvisibleButton("##cb", ImVec2(size, size));
        bool hovered = ImGui::IsItemHovered();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImU32 bg = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        ImU32 border = ImGui::GetColorU32(ImGuiCol_Border);
        dl->AddRectFilled(p, ImVec2(p.x + size, p.y + size), bg, 3.0f);
        dl->AddRect(p, ImVec2(p.x + size, p.y + size), border, 3.0f);

        ImU32 mark = ImGui::GetColorU32(ImGuiCol_CheckMark);
        if (indeterminate) {
            dl->AddLine(ImVec2(p.x + size * 0.25f, p.y + size * 0.5f),
                ImVec2(p.x + size * 0.75f, p.y + size * 0.5f), mark, 2.0f);
        }
        else if (checked) {
            dl->AddLine(ImVec2(p.x + size * 0.20f, p.y + size * 0.55f),
                ImVec2(p.x + size * 0.42f, p.y + size * 0.78f), mark, 2.0f);
            dl->AddLine(ImVec2(p.x + size * 0.42f, p.y + size * 0.78f),
                ImVec2(p.x + size * 0.80f, p.y + size * 0.25f), mark, 2.0f);
        }
        return clicked;
    }

    bool Widgets::ToggleSwitch(const char* id, bool* value)
    {
        ImGui::PushID(id);
        float h = ImGui::GetFrameHeight() * 0.75f;
        float w = h * 1.9f;
        ImVec2 p = ImGui::GetCursorScreenPos();
        bool pressed = ImGui::InvisibleButton("##switch", ImVec2(w, h));
        if (pressed) * value = !* value;
        bool hovered = ImGui::IsItemHovered();

        ImDrawList* dl = ImGui::GetWindowDrawList();
        float r = h * 0.5f;
        ImVec4 track = * value ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) : ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
        if (hovered)
            track = * value ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered)
            : ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered);

        dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), ImGui::GetColorU32(track), r);
        dl->AddRect(p, ImVec2(p.x + w, p.y + h), ImGui::GetColorU32(ImGuiCol_Border), r);
        float knobX = * value ? p.x + w - r : p.x + r;
        dl->AddCircleFilled(ImVec2(knobX, p.y + r), r - 2.5f,
            ImGui::GetColorU32(ImGuiCol_CheckMark));

        ImGui::PopID();
        return pressed;
    }

    bool Widgets::IconButton(const char* id, int kind, const char* tooltip)
    {
        float s = ImGui::GetFrameHeight();
        ImVec2 p = ImGui::GetCursorScreenPos();
        bool pressed = ImGui::InvisibleButton(id, ImVec2(s, s));
        bool hovered = ImGui::IsItemHovered();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        if (hovered)
            dl->AddRectFilled(p, ImVec2(p.x + s, p.y + s),
                ImGui::GetColorU32(ImGuiCol_HeaderHovered, 0.30f), 4.0f);

        ImU32 col = ImGui::GetColorU32(ImGuiCol_Text);
        ImVec2 c(p.x + s * 0.5f, p.y + s * 0.5f);
        if (kind == 0) {  // gear
            dl->AddCircle(c, s * 0.17f, col, 16, 1.5f);
            for (int i = 0; i < 8; i++) {
                float a = i * 3.14159265f / 4.0f;
                ImVec2 d(cosf(a), sinf(a));
                dl->AddLine(ImVec2(c.x + d.x * s * 0.26f, c.y + d.y * s * 0.26f),
                    ImVec2(c.x + d.x * s * 0.36f, c.y + d.y * s * 0.36f), col, 1.5f);
            }
        }
        else {  // trash
            dl->AddRect(ImVec2(c.x - s * 0.15f, c.y - s * 0.10f),
                ImVec2(c.x + s * 0.15f, c.y + s * 0.26f), col, 1.0f, 0, 1.5f);
            dl->AddLine(ImVec2(c.x - s * 0.24f, c.y - s * 0.10f),
                ImVec2(c.x + s * 0.24f, c.y - s * 0.10f), col, 1.5f);
            dl->AddLine(ImVec2(c.x - s * 0.07f, c.y - s * 0.20f),
                ImVec2(c.x + s * 0.07f, c.y - s * 0.20f), col, 1.5f);
        }
        if (hovered && tooltip) ImGui::SetTooltip("%s", tooltip);
        return pressed;
    }

    void Widgets::DrawNodeIcon(ImDrawList* dl, ImVec2 pos, bool folder)
    {
        if (folder) {
            ImU32 col = IM_COL32(220, 180, 70, 255);
            dl->AddRectFilled(ImVec2(pos.x, pos.y + 3), ImVec2(pos.x + 14, pos.y + 13), col, 2.0f);
            dl->AddRectFilled(ImVec2(pos.x + 1, pos.y), ImVec2(pos.x + 6, pos.y + 4), col, 1.0f);
        }
        else {
            ImU32 col = ImGui::GetColorU32(ImGuiCol_TextDisabled);
            dl->AddRect(ImVec2(pos.x + 1, pos.y), ImVec2(pos.x + 13, pos.y + 14), col, 1.0f);
            dl->AddLine(ImVec2(pos.x + 4, pos.y + 4), ImVec2(pos.x + 10, pos.y + 4), col, 1.0f);
            dl->AddLine(ImVec2(pos.x + 4, pos.y + 7), ImVec2(pos.x + 10, pos.y + 7), col, 1.0f);
            dl->AddLine(ImVec2(pos.x + 4, pos.y + 10), ImVec2(pos.x + 8, pos.y + 10), col, 1.0f);
        }
    }

    void Widgets::ChipFlowStatic(const std::vector<std::string>& chips, float width)
    {
        ImVec2 start = ImGui::GetCursorScreenPos();
        const float padX = 6.0f, padY = 5.0f;
        float lineH = ImGui::GetTextLineHeight() + 4.0f;
        float x = start.x + padX, y = start.y + padY;
        float right = start.x + width - padX;
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);
        ImU32 fg = ImGui::GetColorU32(ImGuiCol_Text);

        for (const auto& s : chips) {
            ImVec2 ts = ImGui::CalcTextSize(s.c_str());
            float w = ts.x + 12.0f;
            if (x + w > right && x > start.x + padX) { x = start.x + padX; y += lineH + 3.0f; }
            dl->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + lineH), bg, 3.0f);
            dl->AddText(ImVec2(x + 6.0f, y + (lineH - ts.y) * 0.5f), fg, s.c_str());
            x += w + 4.0f;
        }
        ImGui::SetCursorScreenPos(start);
        ImGui::Dummy(ImVec2(width, (y + lineH + padY) - start.y));
    }

    void Widgets::ChipFlowEditable(std::vector<std::string>& values, float width)
    {
        ImVec2 start = ImGui::GetCursorScreenPos();
        const float padX = 6.0f, padY = 5.0f;
        float lineH = ImGui::GetTextLineHeight() + 4.0f;
        float x = start.x + padX, y = start.y + padY;
        float right = start.x + width - padX;
        int removeIdx = -1;

        for (int i = 0; i < (int)values.size(); i++) {
            std::string label = values[i] + "  x";
            ImVec2 ts = ImGui::CalcTextSize(label.c_str());
            float w = ts.x + 14.0f;
            if (x + w > right && x > start.x + padX) { x = start.x + padX; y += lineH + 3.0f; }

            ImGui::SetCursorScreenPos(ImVec2(x, y));
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 2));
            if (ImGui::Button(label.c_str())) removeIdx = i;
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);
            ImGui::PopID();
            x += w + 4.0f;
        }
        if (removeIdx >= 0) values.erase(values.begin() + removeIdx);

        float totalH = values.empty() ? (lineH + padY * 2.0f) : ((y + lineH + padY) - start.y);
        ImGui::SetCursorScreenPos(start);
        ImGui::Dummy(ImVec2(width, totalH));
    }

    void Widgets::ChipInputBox(const char* id, std::vector<std::string>& values, char* buf, size_t bufSize, const char* placeholder, bool isExtension, const char* helper)
    {
        ImGui::PushID(id);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(Theme::ChipBg));
        ImGui::BeginChild("box", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);

        float w = ImGui::GetContentRegionAvail().x;
        if (!values.empty()) ChipFlowEditable(values, w);

        ImGui::SetNextItemWidth(-1);
        bool enter = ImGui::InputTextWithHint("##input", placeholder, buf, bufSize,
            ImGuiInputTextFlags_EnterReturnsTrue);
        std::string cur(buf);
        bool hasComma = cur.find(',') != std::string::npos;
        if (enter || hasComma) {
            for (auto& p : PathUtils::Split(cur, ',')) CommitValue(values, p, isExtension);
            buf[0] = '\0';
            if (enter) ImGui::SetKeyboardFocusHere(-1);
        }
        if (ImGui::IsItemActive() && buf[0] == '\0' &&
            ImGui::IsKeyPressed(ImGuiKey_Backspace) && !values.empty())
            values.pop_back();

        if (helper && *helper) ImGui::TextDisabled("%s", helper);

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopID();
    }

}