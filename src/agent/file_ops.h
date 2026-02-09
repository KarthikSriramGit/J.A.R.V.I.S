#pragma once

#include <string>
#include <vector>

namespace jarvis {
namespace agent {

std::string read_file(const std::string& path);
bool write_file(const std::string& path, const std::string& content);
std::vector<std::string> list_directory(const std::string& path);
bool file_exists(const std::string& path);

}  // namespace agent
}  // namespace jarvis
