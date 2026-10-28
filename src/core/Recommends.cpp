#include "core/Recommends.h"

namespace code2pdf
{
    const std::vector<std::string>& Recommends::Folders() {
        static const std::vector<std::string> k = {
            "bin","obj","dist","build","out","target","coverage","cache",
            ".parcel-cache",".turbo",".next","next",".svelte-kit","release","debug",
            ".git",".svn",".hg",".vs",".vscode",".idea",
            "node_modules","packages","vendor",".pnpm-store",".yarn",
            "__pycache__",".pytest_cache",".mypy_cache",".ruff_cache",
            "venv",".venv","env",".tox","site-packages","*.egg-info",
            ".gradle",".mvn","*.iml",
            "*.log",".terraform","serverless","tmp","temp",
        };
        return k;
    }

    const std::vector<std::string>& Recommends::Extensions() {
        static const std::vector<std::string> k = {
            "cs","xaml","axaml","fs","fsx","vb","csproj","fsproj","vbproj","sln","props","targets",
            "html","htm","css","scss","sass","less","js","jsx","mjs","cjs",
            "ts","tsx","mts","cts","vue","svelte","astro",
            "java","kt","kts","scala","go","rs","c","cpp","cc","cxx",
            "h","hpp","hh","py","rb","php","pl","lua","dart","swift","m","mm",
            "sh","bash","zsh","fish","ps1","psm1","bat","cmd",
            "json","jsonc","json5","yaml","yml","toml","ini","config","xml",
            "env","properties","gradle",
            "md","mdx","rst","txt","adoc",
            "sql","prisma","graphql","gql","proto",
            "hbs","handlebars","ejs","pug","mustache","liquid","jinja","jinja2",
        };
        return k;
    }
}