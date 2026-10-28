#pragma once
#include <string>
#include <vector>

namespace code2pdf {

    class Recommends {
    public:
        static const std::vector<std::string>& Folders();
        static const std::vector<std::string>& Extensions();
    };

}