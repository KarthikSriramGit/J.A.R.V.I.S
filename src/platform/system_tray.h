#pragma once

#ifdef _WIN32

#include <functional>
#include <atomic>

namespace jarvis {
namespace platform {

class SystemTray {
public:
    using MenuCallback = std::function<void(int id)>;

    SystemTray();
    ~SystemTray();

    bool show();
    void hide();
    void set_menu_callback(MenuCallback cb) { callback_ = std::move(cb); }
    void run_message_loop();

private:
    void* hwnd_ = nullptr;
    void* icon_ = nullptr;
    MenuCallback callback_;
    std::atomic<bool> running_{false};
};

}  // namespace platform
}  // namespace jarvis

#endif
