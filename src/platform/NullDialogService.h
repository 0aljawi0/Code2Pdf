#pragma once
#include "platform/IDialogService.h"

namespace code2pdf 
{

    class NullDialogService : public IDialogService {
    public:
        std::vector<std::string> PickFiles() override { return {}; }
        std::string PickFolder() override { return {}; }
        std::string PickSavePath(const std::string&) override { return {}; }
        std::string SuggestOutputPath(const std::string& source) override {
            return (source.empty() ? "code2pdf" : source) + ".pdf";
        }
        void RevealInExplorer(const std::string&) override {}
    };

}