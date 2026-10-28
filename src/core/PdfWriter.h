#pragma once
#include <string>
#include <vector>

namespace code2pdf 
{
    class PdfWriter 
    {
    public:
        struct Result {
            bool ok = false;
            std::string error;
        };

        Result Generate(const std::string& outputPath, const std::vector<std::string>& files) const;
    };

}