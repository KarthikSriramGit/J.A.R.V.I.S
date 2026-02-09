#include "brain/llm_local.h"
#include "core/logger.h"
#include <fstream>

namespace jarvis {
namespace brain {

bool LLMLocal::load(const std::string& model_path) {
    std::ifstream f(model_path);
    if (!f.good()) {
        JARVIS_LOG_WARN("Local LLM model not found: ", model_path);
        JARVIS_LOG_INFO("llama.cpp integration pending. Using cloud only.");
        return false;
    }
    loaded_ = false;
    JARVIS_LOG_WARN("llama.cpp not integrated. Local LLM disabled.");
    return false;
}

bool LLMLocal::complete(const std::string& system_prompt, const std::string& user_message,
                        std::string& out_response, int max_tokens) {
    (void)system_prompt;
    (void)user_message;
    (void)max_tokens;
    out_response.clear();
    return false;
}

}  // namespace brain
}  // namespace jarvis
