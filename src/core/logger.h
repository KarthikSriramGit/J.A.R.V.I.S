#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>

namespace jarvis {
namespace core {

enum class LogLevel { Debug, Info, Warn, Error };

class Logger {
public:
    static Logger& instance();

    void set_level(LogLevel level) { level_ = level; }
    LogLevel level() const { return level_; }

    void log(LogLevel level, const std::string& msg);

    template<typename... Args>
    void debug(Args&&... args) {
        if (level_ <= LogLevel::Debug)
            log(LogLevel::Debug, format(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void info(Args&&... args) {
        if (level_ <= LogLevel::Info)
            log(LogLevel::Info, format(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void warn(Args&&... args) {
        if (level_ <= LogLevel::Warn)
            log(LogLevel::Warn, format(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void error(Args&&... args) {
        if (level_ <= LogLevel::Error)
            log(LogLevel::Error, format(std::forward<Args>(args)...));
    }

private:
    Logger() = default;
    template<typename... Args>
    static std::string format(Args&&... args) {
        std::ostringstream oss;
        ((oss << std::forward<Args>(args)), ...);
        return oss.str();
    }

    std::string level_str(LogLevel level) const;
    LogLevel level_ = LogLevel::Info;
    std::mutex mutex_;
};

#define JARVIS_LOG_DEBUG(...) ::jarvis::core::Logger::instance().debug(__VA_ARGS__)
#define JARVIS_LOG_INFO(...)  ::jarvis::core::Logger::instance().info(__VA_ARGS__)
#define JARVIS_LOG_WARN(...)  ::jarvis::core::Logger::instance().warn(__VA_ARGS__)
#define JARVIS_LOG_ERROR(...) ::jarvis::core::Logger::instance().error(__VA_ARGS__)

}  // namespace core
}  // namespace jarvis
