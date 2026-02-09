#pragma once

#include <string>
#include <memory>

namespace jarvis {
namespace brain {

class LLMLocal;
class LLMCloud;

class Router {
public:
    Router(std::shared_ptr<LLMLocal> local, std::shared_ptr<LLMCloud> cloud);
    bool route_and_complete(const std::string& system_prompt, const std::string& user_message,
                           std::string& out_response);

private:
    bool should_use_cloud(const std::string& message) const;

    std::shared_ptr<LLMLocal> local_;
    std::shared_ptr<LLMCloud> cloud_;
    int route_threshold_tokens_ = 2000;
};

}  // namespace brain
}  // namespace jarvis
