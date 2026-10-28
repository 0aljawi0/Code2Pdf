#pragma once
#include "imgui.h"

namespace code2pdf {
    class Theme {
    public:
        static void Apply();

        // Spectrum dark accents exposed for UI code that needs explicit colours.
        static constexpr ImU32 Accent = IM_COL32(55, 142, 240, 255); // BLUE400
        static constexpr ImU32 AccentHover = IM_COL32(38, 128, 235, 255); // BLUE500
        static constexpr ImU32 AccentActive = IM_COL32(20, 115, 230, 255); // BLUE600

        static constexpr ImU32 FolderText = IM_COL32(137, 190, 245, 255); // BLUE300
        static constexpr ImU32 Warning = IM_COL32(230, 134, 25, 255); // ORANGE400

        static constexpr ImU32 DangerText = IM_COL32(235, 108, 114, 255); // RED300
        static constexpr ImU32 DangerBg = IM_COL32(215, 55, 63, 56); // RED500 @ ~22%
        static constexpr ImU32 DangerBorder = IM_COL32(215, 55, 63, 153); // RED500 @ ~60%

        static constexpr ImU32 ChipBg = IM_COL32(44, 44, 44, 153); // GRAY200 @ ~60%
    };
}