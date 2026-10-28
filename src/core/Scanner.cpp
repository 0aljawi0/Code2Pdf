#include "core/Scanner.h"
#include "core/PathUtils.h"
#include "core/Recommends.h"

#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

namespace code2pdf
{
	namespace
	{
		fs::path U8Path(const std::string& s) { return fs::u8path(s); }

		std::string PathToU8(const fs::path& p) {
#if defined(__cpp_lib_char8_t)
			auto u8 = p.u8string();
			return std::string(u8.begin(), u8.end());
#else
			return p.u8string();
#endif
		}
	}

    bool Scanner::ShouldSkipDir(const std::string& lowerName,
        const std::vector<std::string>& skip) {
        for (const auto& s : skip) {
            if (s.empty()) continue;
            if (s.front() == '*') {
                const std::string suffix = s.substr(1);
                if (lowerName.size() >= suffix.size() &&
                    lowerName.compare(lowerName.size() - suffix.size(),
                        suffix.size(), suffix) == 0)
                    return true;
            }
            else if (lowerName == s) {
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> Scanner::BuildSkipList(const ScanSettings& s) const {
        std::vector<std::string> out;
        if (s.useRecommendedFolders) out = Recommends::Folders();
        for (const auto& f : s.extraExcludedFolders) {
            std::string t = PathUtils::Trim(f);
            if (!t.empty()) out.push_back(t);
        }
        for (auto& e : out) e = PathUtils::ToLower(e);
        return out;
    }

    std::vector<std::string> Scanner::BuildExtList(const ScanSettings& s) const {
        std::vector<std::string> out;
        if (s.useRecommendedExtensions) out = Recommends::Extensions();
        for (const auto& e : s.extraIncludedExtensions) {
            std::string t = PathUtils::Trim(e);
            while (!t.empty() && t.front() == '.') t.erase(t.begin());
            if (!t.empty()) out.push_back(t);
        }
        for (auto& e : out) e = PathUtils::ToLower(e);
        return out;
    }

    bool Scanner::GetFileEntry(const std::string& path, FileEntry& out) const {
        std::error_code ec;
        auto sz = fs::file_size(U8Path(path), ec);
        if (ec) return false;
        out.path = path;
        out.name = PathUtils::BaseName(path);
        out.size = (uint64_t)sz;
        return true;
    }

    std::vector<FileEntry> Scanner::ScanFolder(const std::string& root,
        const ScanSettings& settings) const {
        std::vector<FileEntry> out;
        const auto skip = BuildSkipList(settings);
        const auto exts = BuildExtList(settings);

        std::error_code ec;
        fs::recursive_directory_iterator it(U8Path(root),
            fs::directory_options::skip_permission_denied, ec), end;
        if (ec) return out;

        for (; it != end; it.increment(ec)) {
            if (ec) { ec.clear(); continue; }
            const fs::directory_entry& de = *it;
            std::error_code fec;

            if (de.is_directory(fec)) {
                const std::string name =
                    PathUtils::ToLower(PathToU8(de.path().filename()));
                if (ShouldSkipDir(name, skip)) it.disable_recursion_pending();
                continue;
            }
            if (!de.is_regular_file(fec)) continue;

            std::string ext = PathUtils::ToLower(PathToU8(de.path().extension()));
            if (!ext.empty() && ext.front() == '.') ext.erase(ext.begin());

            bool match = false;
            for (const auto& e : exts) if (e == ext) { match = true; break; }
            if (!match) continue;

            FileEntry fe;
            if (GetFileEntry(PathToU8(de.path()), fe)) out.push_back(std::move(fe));
        }
        return out;
    }
}