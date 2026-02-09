#include "brain/router.h"
#include "brain/llm_local.h"
#include "brain/llm_cloud.h"
#include "core/config.h"

namespace jarvis {
namespace brain {

Router::Router(std::shared_ptr<LLMLocal> local, std::shared_ptr<LLMCloud> cloud)
    : local_(std::move(local))
    , cloud_(std::move(cloud)) {
    route_threshold_tokens_ = core::Config::instance().route_to_cloud_tokens();
}

bool Router::should_use_cloud(const std::string& message) const {
    if (!cloud_) return false;
    size_t approx_tokens = message.size() / 4;
    if (approx_tokens > static_cast<size_t>(route_threshold_tokens_)) return true;
    const char* keywords[] = {"refactor", "redesign", "architecture", "debug", "multiple files"};
    for (const char* kw : keywords) {
        if (message.find(kw) != std::string::npos) return true;
    }
    return false;
}

bool Router::route_and_complete(const std::string& system_prompt, const std::string& user_message,
                                std::string& out_response) {
    if (should_use_cloud(user_message) && cloud_) {
        return cloud_->complete(system_prompt, user_message, out_response);
    }
    if (local_ && local_->is_loaded()) {
        if (local_->complete(system_prompt, user_message, out_response)) return true;
    }
    if (cloud_) {
        return cloud_->complete(system_prompt, user_message, out_response);
    }
    return false;
}

}  // namespace brain
}  // namespace jarvis
