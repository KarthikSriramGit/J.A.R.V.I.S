#include "audio/player.h"
#include "core/logger.h"
#include <miniaudio.h>
#include <thread>
#include <chrono>
#include <memory>

namespace jarvis {
namespace audio {

Player::Player() {}

Player::~Player() {
    stop();
}

bool Player::play_file(const std::string& path, bool blocking) {
    if (playing_) return false;

    auto decoder = std::make_unique<ma_decoder>();
    ma_result result = ma_decoder_init_file(path.c_str(), nullptr, decoder.get());
    if (result != MA_SUCCESS) {
        JARVIS_LOG_ERROR("Failed to load audio file: ", path, " code=", result);
        return false;
    }

    auto device = std::make_unique<ma_device>();
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder->outputFormat;
    deviceConfig.playback.channels = decoder->outputChannels;
    deviceConfig.sampleRate = decoder->outputSampleRate;
    deviceConfig.pUserData = decoder.get();

    deviceConfig.dataCallback = [](ma_device* pDevice, void* pOutput, const void* pInput,
                                   ma_uint32 frameCount) {
        (void)pInput;
        ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
        if (pDecoder) ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, nullptr);
    };

    result = ma_device_init(nullptr, &deviceConfig, device.get());
    if (result != MA_SUCCESS) {
        JARVIS_LOG_ERROR("Failed to initialize playback device: ", result);
        ma_decoder_uninit(decoder.get());
        return false;
    }

    result = ma_device_start(device.get());
    if (result != MA_SUCCESS) {
        JARVIS_LOG_ERROR("Failed to start playback: ", result);
        ma_device_uninit(device.get());
        ma_decoder_uninit(decoder.get());
        return false;
    }

    playing_ = true;
    auto on_done = on_finished_;

    auto play_to_end = [this, dec = std::move(decoder), dev = std::move(device), on_done]() mutable {
        ma_uint64 total_frames = 0;
        ma_decoder_get_length_in_pcm_frames(dec.get(), &total_frames);
        while (playing_ && total_frames > 0) {
            ma_uint64 pos = 0;
            ma_decoder_get_cursor_in_pcm_frames(dec.get(), &pos);
            if (pos >= total_frames) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        playing_ = false;
        ma_device_uninit(dev.get());
        ma_decoder_uninit(dec.get());
        if (on_done) on_done();
    };

    if (blocking) {
        play_to_end();
    } else {
        std::thread(std::move(play_to_end)).detach();
    }
    return true;
}

void Player::stop() {
    playing_ = false;
}

}  // namespace audio
}  // namespace jarvis
