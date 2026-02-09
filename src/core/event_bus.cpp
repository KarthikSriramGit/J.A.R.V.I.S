#include "core/event_bus.h"
#include "core/logger.h"
#include <algorithm>

namespace jarvis {
namespace core {

EventBus& EventBus::instance() {
    static EventBus inst;
    return inst;
}

void EventBus::subscribe(EventType type, EventHandler handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_.emplace_back(type, std::move(handler));
}

void EventBus::publish(EventType type, const std::string& payload) {
    publish(Event{type, payload});
}

void EventBus::publish(const Event& ev) {
    std::vector<std::pair<EventType, EventHandler>> copy;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(ev);
        copy = handlers_;
    }
    cv_.notify_one();

    for (const auto& [t, h] : copy) {
        if (t == ev.type)
            h(ev);
    }
}

bool EventBus::poll(Event& out) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) return false;
    out = queue_.front();
    queue_.pop();
    return true;
}

void EventBus::run() {
    running_ = true;
    while (running_) {
        Event ev;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait_for(lock, std::chrono::milliseconds(100), [this] {
                return !queue_.empty() || !running_;
            });
            if (!running_) break;
            if (queue_.empty()) continue;
            ev = queue_.front();
            queue_.pop();
        }
        for (const auto& [t, h] : handlers_) {
            if (t == ev.type)
                h(ev);
        }
    }
}

void EventBus::stop() {
    running_ = false;
    cv_.notify_all();
}

}  // namespace core
}  // namespace jarvis
