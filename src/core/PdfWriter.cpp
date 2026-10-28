#include "core/PdfWriter.h"
#include "core/PathUtils.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>

namespace fs = std::filesystem;

namespace code2pdf
{
    namespace {

        fs::path U8Path(const std::string& s) { return fs::u8path(s); }

        std::string PdfEscape(const std::string& s) {
            std::string r;
            r.reserve(s.size() + 8);
            for (char ch : s) {
                unsigned char c = (unsigned char)ch;
                if (ch == '(' || ch == ')' || ch == '\\') { r += '\\'; r += ch; }
                else if (c < 32 || c > 126) r += ' ';
                else r += ch;
            }
            return r;
        }

    }


    PdfWriter::Result PdfWriter::Generate(const std::string& outputPath, const std::vector<std::string>& files) const
    {
        const double PAGE_W = 595.276, PAGE_H = 841.890;
        const double LEFT = 42.52, BOTTOM = 56.69, LINE_H = 9.07;

        std::vector<std::string> pages(1);
        double y = PAGE_H - BOTTOM;

        auto newPage = [&]() { pages.emplace_back(); y = PAGE_H - BOTTOM; };
        auto put = [&](int font, double size, const std::string& text) {
            char buf[160];
            snprintf(buf, sizeof buf, "BT /F%d %.2f Tf 1 0 0 1 %.2f %.2f Tm (",
                font, size, LEFT, y);
            pages.back() += buf;
            pages.back() += PdfEscape(text);
            pages.back() += ") Tj ET\n";
        };

        put(2, 14, "CODE2PDF");
        y -= 20;
        {
            char sub[128];
            snprintf(sub, sizeof sub, "// SOURCE CODE EXPORT * %zu file(s)", files.size());
            put(1, 7, sub);
            y -= 26;
        }

        for (const auto& path : files) {
            std::string content;
            {
                std::ifstream in(U8Path(path), std::ios::binary);
                if (in) content.assign(std::istreambuf_iterator<char>(in),
                    std::istreambuf_iterator<char>());
                else    content = "// (unreadable)";
            }

            if (y < BOTTOM + 46) newPage();

            std::string name = PathUtils::BaseName(path);
            if (name.size() > 85) name.resize(85);
            put(2, 10, name);
            y -= 13;

            std::string shown = path;
            if (shown.size() > 120) shown.resize(120);
            put(1, 6, shown);
            y -= 13;

            int lineNo = 1;
            size_t pos = 0;
            while (pos < content.size()) {
                size_t e = content.find('\n', pos);
                if (e == std::string::npos) e = content.size();
                std::string line = content.substr(pos, e - pos);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                for (char& ch : line) if (ch == '\t') ch = ' ';
                if (line.size() > 115) line.resize(115);

                char num[16];
                snprintf(num, sizeof num, "%4d ", lineNo++);
                if (y < BOTTOM) newPage();
                put(1, 6, std::string(num) + line);
                y -= LINE_H;
                pos = e + 1;
            }
            y -= 12;
        }

        // --- assemble PDF ---
        std::string doc;
        doc.reserve(1 << 20);
        doc += "%PDF-1.4\n%\xE2\xE3\xCF\xD3\n";

        std::vector<size_t> offsets;
        auto obj = [&](const std::string& body) {
            offsets.push_back(doc.size());
            doc += std::to_string(offsets.size()) + " 0 obj\n" + body + "\nendobj\n";
            };

        std::string kids;
        for (size_t i = 0; i < pages.size(); i++)
            kids += std::to_string(5 + 2 * (int)i) + " 0 R ";

        obj("<< /Type /Catalog /Pages 2 0 R >>");
        obj("<< /Type /Pages /Kids [" + kids + "] /Count " +
            std::to_string(pages.size()) + " >>");
        obj("<< /Type /Font /Subtype /Type1 /BaseFont /Courier >>");
        obj("<< /Type /Font /Subtype /Type1 /BaseFont /Courier-Bold >>");

        for (size_t i = 0; i < pages.size(); i++) {
            obj("<< /Type /Page /Parent 2 0 R /MediaBox [0 0 595.276 841.890] "
                "/Resources << /Font << /F1 3 0 R /F2 4 0 R >> >> /Contents " +
                std::to_string(6 + 2 * (int)i) + " 0 R >>");
            obj("<< /Length " + std::to_string(pages[i].size()) + " >>\nstream\n" +
                pages[i] + "endstream");
        }

        const size_t xref = doc.size();
        doc += "xref\n0 " + std::to_string(offsets.size() + 1) + "\n";
        doc += "0000000000 65535 f \n";
        for (size_t off : offsets) {
            char b[32];
            snprintf(b, sizeof b, "%010zu 00000 n \n", off);
            doc += b;
        }
        doc += "trailer\n<< /Size " + std::to_string(offsets.size() + 1) +
            " /Root 1 0 R >>\n";
        doc += "startxref\n" + std::to_string(xref) + "\n%%EOF\n";

        std::ofstream out(U8Path(outputPath), std::ios::binary);
        if (!out) return { false, "Cannot open output file: " + outputPath };
        out.write(doc.data(), (std::streamsize)doc.size());
        if (!out) return { false, "Failed writing output file." };
        return { true, {} };
    }

}