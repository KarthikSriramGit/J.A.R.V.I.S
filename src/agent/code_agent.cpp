#include "agent/code_agent.h"
#include "brain/router.h"
#include "brain/personality.h"
#include "agent/tool_registry.h"
#include "agent/file_ops.h"
#include "agent/shell_exec.h"
#include <nlohmann/json.hpp>

namespace jarvis {
namespace agent {

CodeAgent::CodeAgent(std::shared_ptr<brain::Router> router) : router_(std::move(router)) {}

std::string CodeAgent::process(const std::string& user_request,
                               const std::string& system_prompt) {
    std::string response;
    std::string prompt = system_prompt.empty() ? brain::Personality::jarvis_system_prompt() : system_prompt;
    if (router_->route_and_complete(prompt, user_request, response)) {
        return response;
    }
    return "I'm afraid I'm not quite operational yet, sir. My neural networks are still initializing.";
}

}  // namespace agent
}  // namespace jarvis
