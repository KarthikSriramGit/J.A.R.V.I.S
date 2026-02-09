// Standalone face enrollment utility.
// Build and run: jarvis --enroll <label>
// Captures N frames from webcam and enrolls the face.

#include "vision/camera.h"
#include "vision/face_recognition.h"
#include "core/config.h"
#include "core/logger.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    std::string label = "owner";
    if (argc > 1) label = argv[1];

    jarvis::core::Logger::instance().set_level(jarvis::core::LogLevel::Info);
    if (!jarvis::core::Config::instance().load("config/jarvis.yaml")) {
        std::cerr << "Failed to load config. Run from build directory.\n";
        return 1;
    }

    auto& config = jarvis::core::Config::instance();
    jarvis::vision::FaceRecognition face_recog(
        config.face_landmarks_path(),
        config.face_db_dir(),
        config.face_match_threshold());

    if (!face_recog.load()) {
        std::cerr << "Face recognition failed to load. Run setup.ps1 first.\n";
        return 1;
    }

    jarvis::vision::Camera camera(
        config.camera_device(),
        config.camera_width(),
        config.camera_height(),
        config.camera_fps());

    if (!camera.open()) {
        std::cerr << "Could not open camera.\n";
        return 1;
    }

    int n = config.face_enrollment_samples();
    std::cout << "Look at the camera. Capturing " << n << " samples for '" << label << "'...\n";

    std::vector<cv::Mat> samples;
    cv::Mat frame;
    int collected = 0;
    auto last_capture = std::chrono::steady_clock::now();

    while (collected < n) {
        if (camera.read_frame(frame) && !frame.empty()) {
            cv::Rect rect;
            if (face_recog.detect_face(frame, rect)) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - last_capture).count();
                if (elapsed > 300) {  // 300ms between captures
                    samples.push_back(frame.clone());
                    collected++;
                    last_capture = now;
                    std::cout << "  [" << collected << "/" << n << "] captured.\n";
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    camera.close();

    if (samples.empty()) {
        std::cerr << "No face detected. Enrollment failed.\n";
        return 1;
    }

    face_recog.enroll_face(label, samples);
    std::cout << "Enrollment complete. '" << label << "' added to face database.\n";
    return 0;
}
