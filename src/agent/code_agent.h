#pragma once

#include <string>
#include <memory>
#include <functional>

namespace jarvis {
namespace brain {
class Router;
}

namespace jarvis {
namespace agent {

class CodeAgent {
public:
    explicit CodeAgent(std::shared_ptr<brain::Router> router);
    std::string process(const std::string& user_request,
                        const std::string& system_prompt);

private:
    std::shared_ptr<brain::Router> router_;
};

}  // namespace agent
}  // namespace jarvis
