#include "platform/startup.h"
#include "vision/camera.h"
#include "vision/face_recognition.h"
#include "audio/player.h"
#include "audio/tts.h"
#include "core/config.h"
#include "core/logger.h"
#include "core/event_bus.h"
#include <thread>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;
namespace jarvis {
namespace platform {

StartupSequence::StartupSequence() {}

StartupSequence::~StartupSequence() {
    cancel();
}

void StartupSequence::run() {
    auto& config = core::Config::instance();
    std::string face_db = config.face_db_dir();
    std::string landmarks = config.face_landmarks_path();
    std::string welcome_track = config.welcome_track();

    if (!fs::exists(landmarks)) {
        JARVIS_LOG_WARN("Face landmarks not found. Skipping face recognition.");
        JARVIS_LOG_INFO("Daddy's Home mode disabled. Run setup.ps1 and enroll_face.");
        return;
    }

    vision::FaceRecognition face_recog(landmarks, face_db, config.face_match_threshold());
    if (!face_recog.load()) {
        JARVIS_LOG_WARN("Face recognition failed to load. Skipping welcome sequence.");
        return;
    }

    if (!face_recog.has_enrolled_faces()) {
        JARVIS_LOG_INFO("No enrolled faces. Run enroll_face to enable 'Daddy's Home'.");
        return;
    }

    vision::Camera camera(config.camera_device(),
                         config.camera_width(),
                         config.camera_height(),
                         config.camera_fps());

    if (!camera.open()) {
        JARVIS_LOG_ERROR("Could not open camera. Welcome sequence aborted.");
        return;
    }

    JARVIS_LOG_INFO("Watching for face. Say hello when you're ready, sir.");

    cv::Mat frame;
    std::string label;
    const int max_attempts = 600;  // ~20 seconds at 30fps
    int attempts = 0;

    while (!cancelled_ && attempts < max_attempts) {
        if (camera.read_frame(frame) && !frame.empty()) {
            if (face_recog.recognize_face(frame, label)) {
                JARVIS_LOG_INFO("Face recognized: ", label, ". Welcome home, sir.");
                camera.close();

                if (fs::exists(welcome_track)) {
                    audio::Player player;
                    player.set_on_finished([this]() { on_music_finished(); });
                    player.play_file(welcome_track, true);
                } else {
                    JARVIS_LOG_WARN("Welcome track not found: ", welcome_track);
                    on_music_finished();
                }
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
        ++attempts;
    }

    camera.close();
    JARVIS_LOG_INFO("Face watch timed out. Proceeding without welcome sequence.");
}

void StartupSequence::on_music_finished() {
    std::string greeting = "Welcome home, sir. I've kept the lights on for you.";
    core::EventBus::instance().publish(core::EventType::ResponseReady, greeting);

    auto tts = jarvis::audio::create_text_to_speech();
    if (tts->load("") && tts->is_loaded()) {
        tts->speak(greeting, nullptr);
    }
}

void StartupSequence::cancel() {
    cancelled_ = true;
}

}  // namespace platform
}  // namespace jarvis
