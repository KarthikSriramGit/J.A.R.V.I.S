#pragma once

#include <string>

namespace jarvis {
namespace agent {

std::string execute_shell(const std::string& command, const std::string& cwd = "");

}  // namespace agent
}  // namespace jarvis
