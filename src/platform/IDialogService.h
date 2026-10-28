#pragma once
#include <string>
#include <vector>

namespace code2pdf 
{

    class IDialogService {
    public:
        virtual ~IDialogService() = default;

        virtual std::vector<std::string> PickFiles() = 0;
        virtual std::string PickFolder() = 0;
        virtual std::string PickSavePath(const std::string& suggested) = 0;
        virtual std::string SuggestOutputPath(const std::string& sourceName) = 0;
        virtual void RevealInExplorer(const std::string& path) = 0;
    };

}