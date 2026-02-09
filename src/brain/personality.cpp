#include "brain/personality.h"
#include "core/config.h"
#include <fstream>
#include <sstream>

namespace jarvis {
namespace brain {

std::string Personality::load_system_prompt(const std::string& path) {
    std::ifstream f(path);
    if (!f) return "";
    std::ostringstream oss;
    oss << f.rdbuf();
    return oss.str();
}

std::string Personality::jarvis_system_prompt() {
    auto path = core::Config::instance().assets_dir() + "/prompts/jarvis_system.txt";
    std::string loaded = load_system_prompt(path);
    if (!loaded.empty()) return loaded;
    return R"(You are J.A.R.V.I.S., Tony Stark's AI assistant. Be concise, witty, and address the user as "sir".)";
}

}  // namespace brain
}  // namespace jarvis
