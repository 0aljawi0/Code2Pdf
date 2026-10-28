#include "platform/Win32DialogService.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <cstdio>
#include <ctime>
#include <string>

namespace code2pdf
{
	namespace
	{
        std::wstring Utf8ToWide(const std::string& s) {
            if (s.empty()) return {};
            int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
            std::wstring w(n > 0 ? n - 1 : 0, L'\0');
            if (n > 0) MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, w.data(), n);
            return w;
        }

        std::string WideToUtf8(const std::wstring& w) {
            if (w.empty()) return {};
            int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string s(n > 0 ? n - 1 : 0, '\0');
            if (n > 0) WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, s.data(), n, nullptr, nullptr);
            return s;
        }
	}

    std::vector<std::string> Win32DialogService::PickFiles()
    {
        std::vector<std::string> out;
        static wchar_t buf[65536];
        buf[0] = 0;

        OPENFILENAMEW ofn{};
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = L"All files\0*.*\0";
        ofn.lpstrFile = buf;
        ofn.nMaxFile = 65536;
        ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT |
            OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        if (!GetOpenFileNameW(&ofn)) return out;

        std::wstring first = buf;
        const wchar_t* p = buf + first.size() + 1;
        if (*p == 0) {
            out.push_back(WideToUtf8(first));
        }
        else {
            while (*p) {
                std::wstring f = p;
                out.push_back(WideToUtf8(first + L"\\" + f));
                p += f.size() + 1;
            }
        }

        return out;
    }

    std::string Win32DialogService::PickFolder()
    {
        std::string result;
        IFileOpenDialog* dlg = nullptr;
        if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&dlg))))
            return result;

        DWORD opts = 0;
        dlg->GetOptions(&opts);
        dlg->SetOptions(opts | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);

        if (SUCCEEDED(dlg->Show(nullptr))) {
            IShellItem* item = nullptr;
            if (SUCCEEDED(dlg->GetResult(&item))) {
                PWSTR p = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &p))) {
                    result = WideToUtf8(p);
                    CoTaskMemFree(p);
                }
                item->Release();
            }
        }
        dlg->Release();
        return result;
    }

    std::string Win32DialogService::PickSavePath(const std::string& suggested)
    {
        static wchar_t buf[1024];
        std::wstring w = Utf8ToWide(suggested);
        wcsncpy(buf, w.c_str(), 1023);
        buf[1023] = 0;

        OPENFILENAMEW ofn{};
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = L"PDF document\0*.pdf\0All files\0*.*\0";
        ofn.lpstrFile = buf;
        ofn.nMaxFile = 1024;
        ofn.lpstrDefExt = L"pdf";
        ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT |
            OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
        if (!GetSaveFileNameW(&ofn)) return {};
        return WideToUtf8(buf);
    }

    std::string Win32DialogService::SuggestOutputPath(const std::string& sourceName)
    {
        const std::string base = sourceName.empty() ? "code2pdf" : sourceName;
        std::time_t t = std::time(nullptr);
        std::tm lt{};
        localtime_s(&lt, &t);

        char stamp[64];
        snprintf(stamp, sizeof stamp, "%04d-%02d-%02d_%02d-%02d",
            lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min);
        const std::string file = base + "_" + stamp + ".pdf";

        PWSTR desk = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desk))) {
            std::string dir = WideToUtf8(desk);
            CoTaskMemFree(desk);
            if (!dir.empty()) return dir + "\\" + file;
        }
        return file;
    }

    void Win32DialogService::RevealInExplorer(const std::string& path)
    {
        std::wstring args = L"/select,\"" + Utf8ToWide(path) + L"\"";
        ShellExecuteW(nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
    }

}
#endif // _WIN32
