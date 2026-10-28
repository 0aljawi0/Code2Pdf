#pragma once
#include "imgui.h"
#include <string>
#include <vector>

namespace code2pdf {

    class Widgets {
    public:
        static bool TriCheckbox(bool checked, bool indeterminate, float size);
        static bool ToggleSwitch(const char* id, bool* value);
        static bool IconButton(const char* id, int kind, const char* tooltip);
        static void DrawNodeIcon(ImDrawList* dl, ImVec2 pos, bool folder);
        static void ChipFlowStatic(const std::vector<std::string>& chips, float width);
        static void ChipFlowEditable(std::vector<std::string>& values, float width);
        static void ChipInputBox(
            const char* id,
            std::vector<std::string>& values,
            char* buf, size_t bufSize,
            const char* placeholder,
            bool isExtension,
            const char* helper
        );
    };

}