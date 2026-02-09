#include "vision/camera.h"
#include "core/logger.h"
#include <chrono>

namespace jarvis {
namespace vision {

Camera::Camera(int device_index, int width, int height, int fps)
    : device_index_(device_index)
    , width_(width)
    , height_(height)
    , fps_(fps) {}

Camera::~Camera() {
    stop_capture_loop();
    close();
}

bool Camera::open() {
    if (cap_.isOpened()) return true;
    if (!cap_.open(device_index_)) {
        JARVIS_LOG_ERROR("Failed to open camera device ", device_index_);
        return false;
    }
    cap_.set(cv::CAP_PROP_FRAME_WIDTH, width_);
    cap_.set(cv::CAP_PROP_FRAME_HEIGHT, height_);
    cap_.set(cv::CAP_PROP_FPS, fps_);
    JARVIS_LOG_INFO("Camera opened: ", width_, "x", height_, " @ ", fps_, " fps");
    return true;
}

void Camera::close() {
    cap_.release();
}

bool Camera::read_frame(cv::Mat& frame) {
    if (!cap_.isOpened()) return false;
    return cap_.read(frame);
}

void Camera::start_capture_loop(std::function<void(const cv::Mat&)> on_frame) {
    if (capturing_) return;
    if (!cap_.isOpened() && !open()) return;

    capturing_ = true;
    capture_thread_ = std::thread([this, on_frame]() {
        cv::Mat frame;
        auto frame_interval = std::chrono::milliseconds(1000 / (fps_ > 0 ? fps_ : 30));
        while (capturing_ && cap_.isOpened()) {
            if (cap_.read(frame) && !frame.empty()) {
                on_frame(frame);
            }
            std::this_thread::sleep_for(frame_interval);
        }
    });
}

void Camera::stop_capture_loop() {
    capturing_ = false;
    if (capture_thread_.joinable())
        capture_thread_.join();
}

}  // namespace vision
}  // namespace jarvis
