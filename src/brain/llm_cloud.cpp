#include "brain/llm_cloud.h"
#include "core/logger.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <sstream>

namespace jarvis {
namespace brain {

struct LLMCloud::Impl {
    std::string api_key;
    std::string model;
    httplib::SSLClient client{"api.openai.com", 443};
};

LLMCloud::LLMCloud(const std::string& api_key, const std::string& model)
    : impl_(std::make_unique<Impl>()) {
    impl_->api_key = api_key;
    impl_->model = model;
}

LLMCloud::~LLMCloud() = default;

bool LLMCloud::complete(const std::string& system_prompt, const std::string& user_message,
                        std::string& out_response, int max_tokens) {
    nlohmann::json messages = nlohmann::json::array();
    if (!system_prompt.empty()) {
        messages.push_back({{"role", "system"}, {"content", system_prompt}});
    }
    messages.push_back({{"role", "user"}, {"content", user_message}});

    nlohmann::json body = {
        {"model", impl_->model},
        {"max_tokens", max_tokens},
        {"messages", messages}
    };

    httplib::Headers headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + impl_->api_key}
    };

    auto res = impl_->client.Post("/v1/chat/completions", headers, body.dump(), "application/json");

    if (!res || res->status != 200) {
        JARVIS_LOG_ERROR("OpenAI API error: ", res ? res->status : -1, " ", res ? res->body : "");
        return false;
    }

    try {
        auto j = nlohmann::json::parse(res->body);
        if (j.contains("choices") && !j["choices"].empty()) {
            out_response = j["choices"][0]["message"]["content"].get<std::string>();
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        JARVIS_LOG_ERROR("OpenAI parse error: ", e.what());
        return false;
    }
}

void LLMCloud::complete_stream(const std::string& system_prompt, const std::string& user_message,
                               StreamCallback on_chunk, int max_tokens) {
    nlohmann::json messages = nlohmann::json::array();
    if (!system_prompt.empty()) {
        messages.push_back({{"role", "system"}, {"content", system_prompt}});
    }
    messages.push_back({{"role", "user"}, {"content", user_message}});

    nlohmann::json body = {
        {"model", impl_->model},
        {"max_tokens", max_tokens},
        {"stream", true},
        {"messages", messages}
    };

    httplib::Headers headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + impl_->api_key}
    };

    auto res = impl_->client.Post("/v1/chat/completions", headers, body.dump(), "application/json",
        [on_chunk](const char* data, size_t len) {
            std::string chunk(data, len);
            if (chunk.find("\"delta\"") != std::string::npos && chunk.find("\"content\"") != std::string::npos) {
                try {
                    auto j = nlohmann::json::parse(chunk);
                    if (j.contains("choices") && !j["choices"].empty() &&
                        j["choices"][0].contains("delta") &&
                        j["choices"][0]["delta"].contains("content")) {
                        on_chunk(j["choices"][0]["delta"]["content"].get<std::string>());
                    }
                } catch (...) {}
            }
            return true;
        });

    (void)res;
}

}  // namespace brain
}  // namespace jarvis
