#include "audio/tts.h"
#include "core/logger.h"
#include <thread>

#ifdef _WIN32
#include <sapi.h>
#include <sphelper.h>
#pragma comment(lib, "sapi.lib")
#endif

namespace jarvis {
namespace audio {

class TtsWindows : public TextToSpeech {
public:
    TtsWindows() = default;
    ~TtsWindows() override { stop(); }

    bool load(const std::string& model_path) override {
        (void)model_path;
#ifdef _WIN32
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        HRESULT hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL,
            IID_ISpVoice, (void**)&voice_);
        if (SUCCEEDED(hr) && voice_) {
            loaded_ = true;
            JARVIS_LOG_INFO("TTS: Using Windows SAPI (British voice if available).");
            return true;
        }
#endif
        JARVIS_LOG_WARN("TTS: Windows SAPI failed. Piper integration pending.");
        return false;
    }

    bool is_loaded() const override { return loaded_; }

    void speak(const std::string& text, DoneCallback on_done) override {
        if (!loaded_ || text.empty()) {
            if (on_done) on_done();
            return;
        }
#ifdef _WIN32
        std::thread([this, text, on_done]() {
            CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            WCHAR wtext[4096];
            MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, wtext, 4096);
            if (voice_) {
                voice_->Speak(wtext, SVSFDefault, nullptr);
            }
            CoUninitialize();
            if (on_done) on_done();
        }).detach();
#else
        JARVIS_LOG_DEBUG("TTS speak: ", text);
        if (on_done) on_done();
#endif
    }

    void stop() override {
#ifdef _WIN32
        if (voice_) {
            voice_->Speak(nullptr, SVSFPurgeBeforeSpeak, nullptr);
        }
#endif
    }

private:
    bool loaded_ = false;
#ifdef _WIN32
    ISpVoice* voice_ = nullptr;
#endif
};

std::unique_ptr<TextToSpeech> create_text_to_speech() {
    return std::make_unique<TtsWindows>();
}

}  // namespace audio
}  // namespace jarvis
