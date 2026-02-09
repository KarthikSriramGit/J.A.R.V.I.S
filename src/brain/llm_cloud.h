#pragma once

#include <string>
#include <functional>
#include <memory>

namespace jarvis {
namespace brain {

class LLMCloud {
public:
    LLMCloud(const std::string& api_key, const std::string& model = "gpt-4o-mini");
    ~LLMCloud();

    using StreamCallback = std::function<void(const std::string& chunk)>;
    bool complete(const std::string& system_prompt, const std::string& user_message,
                  std::string& out_response, int max_tokens = 1024);
    void complete_stream(const std::string& system_prompt, const std::string& user_message,
                        StreamCallback on_chunk, int max_tokens = 1024);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace brain
}  // namespace jarvis
