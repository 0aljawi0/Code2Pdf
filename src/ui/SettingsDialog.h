#pragma once
#include "core/Models.h"

namespace code2pdf 
{

    class SettingsDialog {
    public:
        void Open(const ScanSettings& current);
        // Returns true if "Save" was clicked; on true, `savedOut` holds the new settings.
        bool Draw(ScanSettings& savedOut);

    private:
        ScanSettings draft_;
        char folderDraft_[256] = "";
        char extDraft_[256] = "";
        bool pendingOpen_ = false;
    };

}