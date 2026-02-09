#pragma once

#include <string>

namespace jarvis {
namespace brain {

class Personality {
public:
    static std::string load_system_prompt(const std::string& path);
    static std::string jarvis_system_prompt();
};

}  // namespace brain
}  // namespace jarvis
