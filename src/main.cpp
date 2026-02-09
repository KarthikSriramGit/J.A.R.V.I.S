#include "core/config.h"
#include "core/logger.h"
#include "core/orchestrator.h"
#include "core/event_bus.h"
#include "platform/startup.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <thread>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    using namespace jarvis::core;

    std::string config_path = "config/jarvis.yaml";
    if (argc > 1) config_path = argv[1];

    if (!fs::exists(config_path)) {
        std::cerr << "Config not found: " << config_path << "\n";
        std::cerr << "Run from build directory with config/ and assets/ copied.\n";
        return 1;
    }

    Logger::instance().set_level(LogLevel::Debug);
    JARVIS_LOG_INFO("J.A.R.V.I.S. — Just A Rather Very Intelligent System");
    JARVIS_LOG_INFO("Initializing...");

    if (!Config::instance().load(config_path)) {
        JARVIS_LOG_ERROR("Failed to load config. Exiting.");
        return 1;
    }

    std::thread startup_thread([]() {
        jarvis::platform::StartupSequence seq;
        seq.run();
    });
    startup_thread.detach();

    Orchestrator orch;
    orch.start();

    return 0;
}
