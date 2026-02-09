#pragma once

#include <memory>
#include <atomic>
#include <functional>

namespace jarvis {
namespace platform {

class StartupSequence {
public:
    StartupSequence();
    ~StartupSequence();

    void run();
    void cancel();

private:
    void run_welcome_sequence();
    void on_music_finished();

    std::atomic<bool> cancelled_{false};
};

}  // namespace platform
}  // namespace jarvis
