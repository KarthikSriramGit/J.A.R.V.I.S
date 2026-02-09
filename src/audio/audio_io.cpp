#include "audio/audio_io.h"
#include "core/logger.h"
#include <miniaudio.h>
#include <cstring>

namespace jarvis {
namespace audio {

static void ma_data_callback(ma_device* pDevice, void* pOutput, const void* pInput,
                             ma_uint32 frameCount) {
    (void)pOutput;
    auto* capture = static_cast<AudioCapture*>(pDevice->pUserData);
    if (capture && pInput) {
        size_t count = frameCount * pDevice->capture.channels;
        capture->on_audio_data(static_cast<const float*>(pInput), count);
    }
}

void AudioCapture::on_audio_data(const float* samples, size_t count) {
    if (callback_) callback_(samples, count);
}

AudioCapture::AudioCapture(int sample_rate, int channels, int device_index)
    : sample_rate_(sample_rate)
    , channels_(channels)
    , device_index_(device_index) {}

AudioCapture::~AudioCapture() {
    stop();
}

bool AudioCapture::start() {
    if (capturing_) return true;

    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format = ma_format_f32;
    config.capture.channels = channels_;
    config.sampleRate = sample_rate_;
    config.dataCallback = ma_data_callback;
    config.pUserData = this;

    ma_device* dev = new ma_device();
    ma_result result = ma_device_init(nullptr, &config, dev);
    if (result != MA_SUCCESS) {
        JARVIS_LOG_ERROR("Failed to init capture device: ", result);
        delete dev;
        return false;
    }

    result = ma_device_start(dev);
    if (result != MA_SUCCESS) {
        JARVIS_LOG_ERROR("Failed to start capture: ", result);
        ma_device_uninit(dev);
        delete dev;
        return false;
    }

    device_ = dev;
    capturing_ = true;
    JARVIS_LOG_INFO("Audio capture started: ", sample_rate_, " Hz, ", channels_, " ch");
    return true;
}

void AudioCapture::stop() {
    if (!capturing_) return;
    capturing_ = false;
    if (device_) {
        ma_device* dev = static_cast<ma_device*>(device_);
        ma_device_uninit(dev);
        delete dev;
        device_ = nullptr;
    }
}

}  // namespace audio
}  // namespace jarvis
