#pragma once

#include <string>
#include <functional>
#include <atomic>
#include <thread>

namespace jarvis {
namespace audio {

class Player {
public:
    Player();
    ~Player();

    bool play_file(const std::string& path, bool blocking = false);
    void stop();
    bool is_playing() const { return playing_; }

    void set_on_finished(std::function<void()> cb) { on_finished_ = std::move(cb); }

private:
    std::function<void()> on_finished_;
    std::atomic<bool> playing_{false};
};

}  // namespace audio
}  // namespace jarvis
