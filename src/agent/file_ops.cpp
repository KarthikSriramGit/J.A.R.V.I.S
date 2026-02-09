#include "agent/file_ops.h"
#include "core/logger.h"
#include <fstream>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;
namespace jarvis {
namespace agent {

std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) {
        JARVIS_LOG_WARN("Cannot read file: ", path);
        return "";
    }
    std::ostringstream oss;
    oss << f.rdbuf();
    return oss.str();
}

bool write_file(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    if (!f) {
        JARVIS_LOG_WARN("Cannot write file: ", path);
        return false;
    }
    f << content;
    return true;
}

std::vector<std::string> list_directory(const std::string& path) {
    std::vector<std::string> out;
    try {
        for (const auto& e : fs::directory_iterator(path)) {
            out.push_back(e.path().filename().string());
        }
    } catch (const std::exception& e) {
        JARVIS_LOG_WARN("list_dir error: ", e.what());
    }
    return out;
}

bool file_exists(const std::string& path) {
    return fs::exists(path);
}

}  // namespace agent
}  // namespace jarvis
