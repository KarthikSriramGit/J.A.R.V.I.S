#include "agent/code_search.h"
#include "agent/shell_exec.h"
#include <sstream>
#include <regex>

namespace jarvis {
namespace agent {

std::vector<SearchResult> search_code(const std::string& pattern, const std::string& path) {
    std::vector<SearchResult> out;
    std::string cmd = "rg --line-number \"" + pattern + "\" " + path;
    std::string result = execute_shell(cmd, path.empty() ? "." : path);
    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line)) {
        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string file = line.substr(0, colon);
        size_t colon2 = line.find(':', colon + 1);
        if (colon2 == std::string::npos) continue;
        int line_num = 0;
        try {
            line_num = std::stoi(line.substr(colon + 1, colon2 - colon - 1));
        } catch (...) { continue; }
        std::string content = line.substr(colon2 + 1);
        out.push_back({file, line_num, content});
    }
    return out;
}

}  // namespace agent
}  // namespace jarvis
