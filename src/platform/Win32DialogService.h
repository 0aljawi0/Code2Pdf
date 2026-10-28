#pragma once
#include "platform/IDialogService.h"

namespace code2pdf 
{
    class Win32DialogService : public IDialogService {
    public:
        std::vector<std::string> PickFiles() override;
        std::string PickFolder() override;
        std::string PickSavePath(const std::string& suggested) override;
        std::string SuggestOutputPath(const std::string& sourceName) override;
        void RevealInExplorer(const std::string& path) override;
    };
}