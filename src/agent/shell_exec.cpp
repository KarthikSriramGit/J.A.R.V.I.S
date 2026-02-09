#include "agent/shell_exec.h"
#include "core/logger.h"
#include <array>
#include <cstdio>
#include <memory>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace jarvis {
namespace agent {

std::string execute_shell(const std::string& command, const std::string& cwd) {
    std::string cmd = command;
#ifdef _WIN32
    if (!cwd.empty()) {
        cmd = "cd /d \"" + cwd + "\" && " + command;
    }
    cmd += " 2>&1";
#else
    if (!cwd.empty()) {
        cmd = "cd \"" + cwd + "\" && " + command;
    }
    cmd += " 2>&1";
#endif
    std::array<char, 4096> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        JARVIS_LOG_WARN("popen failed: ", command);
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

}  // namespace agent
}  // namespace jarvis
