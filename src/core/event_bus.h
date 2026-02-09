#pragma once

#include <string>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace jarvis {
namespace core {

enum class EventType {
    FaceDetected,
    FaceRecognized,
    VoiceInput,
    CommandParsed,
    ResponseReady,
    PlayAudio,
    EnterQuietMode,
    WakeUp,
    Shutdown
};

struct Event {
    EventType type;
    std::string payload;
};

using EventHandler = std::function<void(const Event&)>;

class EventBus {
public:
    static EventBus& instance();

    void subscribe(EventType type, EventHandler handler);
    void publish(EventType type, const std::string& payload = "");
    void publish(const Event& ev);

    bool poll(Event& out);
    void run();
    void stop();

private:
    EventBus() = default;
    std::vector<std::pair<EventType, EventHandler>> handlers_;
    std::queue<Event> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{true};
};

}  // namespace core
}  // namespace jarvis
