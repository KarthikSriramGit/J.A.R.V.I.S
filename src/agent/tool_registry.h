#pragma once

#include <string>
#include <vector>
#include <functional>
#include <nlohmann/json.hpp>

namespace jarvis {
namespace agent {

using ToolHandler = std::function<std::string(const nlohmann::json& args)>;

struct ToolDef {
    std::string name;
    std::string description;
    nlohmann::json parameters;
    ToolHandler handler;
};

class ToolRegistry {
public:
    void register_tool(const ToolDef& def);
    std::string invoke(const std::string& name, const nlohmann::json& args) const;
    std::string to_openai_schema() const;

private:
    std::vector<ToolDef> tools_;
};

}  // namespace agent
}  // namespace jarvis
