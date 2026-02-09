#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace jarvis {
namespace audio {

class TextToSpeech {
public:
    using DoneCallback = std::function<void()>;

    virtual ~TextToSpeech() = default;
    virtual bool load(const std::string& model_path) = 0;
    virtual bool is_loaded() const = 0;
    virtual void speak(const std::string& text, DoneCallback on_done = nullptr) = 0;
    virtual void stop() = 0;
};

std::unique_ptr<TextToSpeech> create_text_to_speech();

}  // namespace audio
}  // namespace jarvis
