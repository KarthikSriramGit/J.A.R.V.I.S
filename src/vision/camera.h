#pragma once

#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <string>
#include <memory>
#include <functional>
#include <atomic>
#include <thread>

namespace jarvis {
namespace vision {

class Camera {
public:
    Camera(int device_index = 0, int width = 640, int height = 480, int fps = 30);
    ~Camera();

    bool open();
    void close();
    bool is_open() const { return cap_.isOpened(); }

    bool read_frame(cv::Mat& frame);
    void start_capture_loop(std::function<void(const cv::Mat&)> on_frame);
    void stop_capture_loop();

private:
    cv::VideoCapture cap_;
    int device_index_;
    int width_, height_, fps_;
    std::atomic<bool> capturing_{false};
    std::thread capture_thread_;
};

}  // namespace vision
}  // namespace jarvis
