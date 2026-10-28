#include "core/PathUtils.h"
#include <algorithm>
#include <cctype>
#include <cstdio>

namespace code2pdf
{
	std::string PathUtils::ToLower(std::string s)
	{
		for (auto& c : s) c = (char)std::tolower((unsigned char)c);
		return s;
	}

	std::string PathUtils::Trim(const std::string& s) {
		size_t a = s.find_first_not_of(" \t\r\n");
		if (a == std::string::npos) return {};
		size_t b = s.find_last_not_of(" \t\r\n");
		return s.substr(a, b - a + 1);
	}

	std::string PathUtils::ToPosix(std::string p) {
		std::replace(p.begin(), p.end(), '\\', '/');
		return p;
	}

	std::string PathUtils::BaseName(const std::string& p) {
		size_t i = p.find_last_of("/\\");
		return i == std::string::npos ? p : p.substr(i + 1);
	}

	std::string PathUtils::FormatSize(uint64_t bytes) {
		char b[64];
		if (bytes < 1024) snprintf(b, sizeof b, "%llu B", (unsigned long long)bytes);
		else if (bytes < 1024ull * 1024ull) snprintf(b, sizeof b, "%.1f KB", bytes / 1024.0);
		else snprintf(b, sizeof b, "%.1f MB", bytes / (1024.0 * 1024.0));
		return b;
	}

	std::vector<std::string> PathUtils::Split(const std::string& s, char sep) {
		std::vector<std::string> out;
		size_t start = 0;
		for (size_t i = 0; i <= s.size(); i++) {
			if (i == s.size() || s[i] == sep) {
				if (i > start) out.push_back(s.substr(start, i - start));
				start = i + 1;
			}
		}
		return out;
	}
}