#pragma once

#include <string>
#include <atomic>
#include <memory>

namespace jarvis {
namespace core {

enum class OrchestratorState {
    Initializing,
    WatchingForFace,
    PlayingWelcome,
    ActiveListening,
    ProcessingCommand,
    QuietMode,
    ShuttingDown
};

class Orchestrator {
public:
    Orchestrator();
    ~Orchestrator();

    void start();
    void stop();
    bool running() const { return running_; }

    OrchestratorState state() const { return state_; }
    void set_state(OrchestratorState s) { state_ = s; }

    void request_quiet_mode();
    void request_wake();

private:
    void run_loop();
    void on_face_recognized(const std::string& payload);
    void on_voice_input(const std::string& payload);

    std::atomic<bool> running_{false};
    std::atomic<OrchestratorState> state_{OrchestratorState::Initializing};
};

}  // namespace core
}  // namespace jarvis
