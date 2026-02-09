#include "agent/tool_registry.h"
#include <nlohmann/json.hpp>

namespace jarvis {
namespace agent {

void ToolRegistry::register_tool(const ToolDef& def) {
    tools_.push_back(def);
}

std::string ToolRegistry::invoke(const std::string& name, const nlohmann::json& args) const {
    for (const auto& t : tools_) {
        if (t.name == name && t.handler) {
            return t.handler(args);
        }
    }
    return R"({"error": "Unknown tool: )" + name + "\"}";
}

std::string ToolRegistry::to_openai_schema() const {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& t : tools_) {
        arr.push_back({
            {"type", "function"},
            {"function", {
                {"name", t.name},
                {"description", t.description},
                {"parameters", t.parameters}
            }}
        });
    }
    return arr.dump();
}

}  // namespace agent
}  // namespace jarvis
