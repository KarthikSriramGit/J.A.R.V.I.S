#pragma once

#include <string>
#include <functional>
#include <memory>

namespace jarvis {
namespace brain {

class LLMLocal {
public:
    LLMLocal() = default;
    ~LLMLocal() = default;

    bool load(const std::string& model_path);
    bool is_loaded() const { return loaded_; }
    bool complete(const std::string& system_prompt, const std::string& user_message,
                  std::string& out_response, int max_tokens = 512);

private:
    bool loaded_ = false;
};

}  // namespace brain
}  // namespace jarvis
