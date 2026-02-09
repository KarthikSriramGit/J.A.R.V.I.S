#pragma once

#include <string>
#include <vector>

namespace jarvis {
namespace agent {

struct SearchResult {
    std::string file;
    int line;
    std::string content;
};

std::vector<SearchResult> search_code(const std::string& pattern, const std::string& path = ".");

}  // namespace agent
}  // namespace jarvis
