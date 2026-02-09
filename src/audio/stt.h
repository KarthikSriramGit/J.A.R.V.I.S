#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace jarvis {
namespace audio {

class SpeechToText {
public:
    using ResultCallback = std::function<void(const std::string& text)>;

    virtual ~SpeechToText() = default;
    virtual bool load(const std::string& model_path) = 0;
    virtual bool is_loaded() const = 0;
    virtual void process_audio(const float* samples, size_t count) = 0;
    virtual void flush(ResultCallback on_result) = 0;
    virtual void reset() = 0;
};

std::unique_ptr<SpeechToText> create_speech_to_text();

}  // namespace audio
}  // namespace jarvis
