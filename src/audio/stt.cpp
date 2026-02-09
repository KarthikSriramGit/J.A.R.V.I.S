#include "audio/stt.h"
#include "core/logger.h"
#include <deque>
#include <mutex>

namespace jarvis {
namespace audio {

class SttStub : public SpeechToText {
public:
    bool load(const std::string& model_path) override {
        (void)model_path;
        JARVIS_LOG_WARN("STT stub: whisper.cpp not integrated. Speech recognition disabled.");
        return false;
    }
    bool is_loaded() const override { return false; }
    void process_audio(const float* samples, size_t count) override { (void)samples; (void)count; }
    void flush(ResultCallback on_result) override {
        if (on_result) on_result("");
    }
    void reset() override {}
};

std::unique_ptr<SpeechToText> create_speech_to_text() {
    return std::make_unique<SttStub>();
}

}  // namespace audio
}  // namespace jarvis
