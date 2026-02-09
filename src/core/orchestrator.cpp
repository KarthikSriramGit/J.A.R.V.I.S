#include "core/orchestrator.h"
#include "core/event_bus.h"
#include "core/config.h"
#include "core/logger.h"
#include <thread>
#include <chrono>

namespace jarvis {
namespace core {

Orchestrator::Orchestrator() {}

Orchestrator::~Orchestrator() {
    stop();
}

void Orchestrator::start() {
    if (running_) return;
    running_ = true;
    state_ = OrchestratorState::Initializing;

    EventBus::instance().subscribe(EventType::FaceRecognized,
        [this](const Event& ev) { on_face_recognized(ev.payload); });
    EventBus::instance().subscribe(EventType::VoiceInput,
        [this](const Event& ev) { on_voice_input(ev.payload); });

    JARVIS_LOG_INFO("J.A.R.V.I.S. orchestrator starting...");
    state_ = OrchestratorState::WatchingForFace;
    run_loop();
}

void Orchestrator::stop() {
    running_ = false;
    state_ = OrchestratorState::ShuttingDown;
    EventBus::instance().stop();
    JARVIS_LOG_INFO("J.A.R.V.I.S. orchestrator stopped.");
}

void Orchestrator::run_loop() {
    while (running_) {
        Event ev;
        if (EventBus::instance().poll(ev)) {
            switch (ev.type) {
                case EventType::Shutdown:
                    running_ = false;
                    break;
                default:
                    break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void Orchestrator::on_face_recognized(const std::string& payload) {
    (void)payload;
    if (state_ != OrchestratorState::WatchingForFace) return;
    JARVIS_LOG_INFO("Face recognized. Playing welcome sequence.");
    state_ = OrchestratorState::PlayingWelcome;
    EventBus::instance().publish(EventType::PlayAudio,
        Config::instance().welcome_track());
}

void Orchestrator::on_voice_input(const std::string& payload) {
    (void)payload;
    if (state_ != OrchestratorState::ActiveListening) return;
    state_ = OrchestratorState::ProcessingCommand;
}

void Orchestrator::request_quiet_mode() {
    state_ = OrchestratorState::QuietMode;
    JARVIS_LOG_INFO("Entering quiet mode. Standing down, sir.");
}

void Orchestrator::request_wake() {
    if (state_ == OrchestratorState::QuietMode) {
        state_ = OrchestratorState::ActiveListening;
        JARVIS_LOG_INFO("Back online. At your service, sir.");
    }
}

}  // namespace core
}  // namespace jarvis
