#pragma once

#include <vector>
#include <cstdint>
#include <functional>
#include <atomic>

namespace jarvis {
namespace audio {

class AudioCapture {
public:
    AudioCapture(int sample_rate = 16000, int channels = 1, int device_index = -1);
    ~AudioCapture();

    bool start();
    void stop();
    bool is_capturing() const { return capturing_; }

    using Callback = std::function<void(const float* samples, size_t count)>;
    void set_callback(Callback cb) { callback_ = std::move(cb); }

    void on_audio_data(const float* samples, size_t count);

private:
    int sample_rate_;
    int channels_;
    int device_index_;
    Callback callback_;
    std::atomic<bool> capturing_{false};
    void* device_ = nullptr;
};

}  // namespace audio
}  // namespace jarvis
