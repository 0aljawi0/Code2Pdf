#pragma once
#include "platform/IDialogService.h"
#include <string>

namespace code2pdf 
{

    class SuccessDialog {
    public:
        explicit SuccessDialog(IDialogService& dialogs) : dialogs_(dialogs) {}

        void Open(const std::string& outputPath);
        void Draw();

    private:
        IDialogService& dialogs_;
        std::string     outputPath_;
        bool            pendingOpen_ = false;
    };
}