#pragma once

#include <string>
#include <optional>
#include <unordered_map>

namespace jarvis {
namespace core {

class Config {
public:
    static Config& instance();

    bool load(const std::string& path);
    bool loaded() const { return loaded_; }

    std::string models_dir() const;
    std::string assets_dir() const;
    std::string face_db_dir() const;

    std::string whisper_model_path() const;
    std::string llm_model_path() const;
    std::string tts_model_path() const;
    std::string face_landmarks_path() const;

    std::string welcome_track() const;
    int sample_rate() const;
    double vad_threshold() const;
    int mic_device_index() const;

    int camera_device() const;
    int camera_width() const;
    int camera_height() const;
    int camera_fps() const;

    double face_match_threshold() const;
    int face_enrollment_samples() const;

    std::string cloud_api_key_env() const;
    std::string cloud_api_key() const;  
    int local_context_size() const;
    std::string cloud_model() const;
    int route_to_cloud_tokens() const;

    std::string user_name() const;
    std::string get(const std::string& key, const std::string& default_val = "") const;
    int get_int(const std::string& key, int default_val = 0) const;
    double get_double(const std::string& key, double default_val = 0) const;

private:
    Config() = default;
    bool loaded_ = false;
    std::string base_path_;
    std::string config_dir_;
    std::string models_root_;
    std::unordered_map<std::string, std::string> kv_;
};

}  // namespace core
}  // namespace jarvis
