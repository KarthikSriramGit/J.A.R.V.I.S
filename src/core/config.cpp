#include "core/config.h"
#include "core/logger.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;
namespace jarvis {
namespace core {

Config& Config::instance() {
    static Config inst;
    return inst;
}

static void flatten(const YAML::Node& node, const std::string& prefix,
                    std::unordered_map<std::string, std::string>& out) {
    if (node.IsMap()) {
        for (const auto& p : node) {
            std::string key = prefix.empty() ? p.first.Scalar() : prefix + "." + p.first.Scalar();
            flatten(p.second, key, out);
        }
    } else if (node.IsScalar()) {
        out[prefix] = node.Scalar();
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); ++i) {
            flatten(node[i], prefix + "[" + std::to_string(i) + "]", out);
        }
    }
}

bool Config::load(const std::string& path) {
    try {
        YAML::Node root = YAML::LoadFile(path);
        kv_.clear();
        flatten(root, "", kv_);

        auto config_dir = fs::path(path).parent_path();
        config_dir_ = config_dir.string();
        base_path_ = config_dir.parent_path().string();
        if (base_path_.empty()) base_path_ = ".";
        models_root_ = get("paths.root", ".");

        loaded_ = true;
        JARVIS_LOG_INFO("Config loaded from ", path);
        return true;
    } catch (const std::exception& e) {
        JARVIS_LOG_ERROR("Failed to load config: ", e.what());
        return false;
    }
}

std::string Config::models_dir() const {
    return (fs::path(base_path_) / models_root_ / get("paths.models_dir", "models")).lexically_normal().string();
}

std::string Config::assets_dir() const {
    return (fs::path(base_path_) / get("paths.assets_dir", "assets")).string();
}

std::string Config::face_db_dir() const {
    return (fs::path(base_path_) / get("paths.face_db_dir", "assets/face_db")).string();
}

std::string Config::whisper_model_path() const {
    return (fs::path(models_dir()) / get("models.whisper", "whisper/ggml-base.en.bin")).string();
}

std::string Config::llm_model_path() const {
    return (fs::path(models_dir()) / get("models.llm", "llm/qwen2.5-7b-instruct-q4_k_m.gguf")).string();
}

std::string Config::tts_model_path() const {
    return (fs::path(models_dir()) / get("models.tts", "tts/en_GB-alan-medium.onnx")).string();
}

std::string Config::face_landmarks_path() const {
    return (fs::path(models_dir()) / get("models.face_landmarks", "face/shape_predictor_68_face_landmarks.dat")).string();
}

std::string Config::welcome_track() const {
    return (fs::path(base_path_) / get("audio.welcome_track", "assets/audio/should_i_stay.mp3")).string();
}

int Config::sample_rate() const { return get_int("audio.sample_rate", 16000); }
double Config::vad_threshold() const { return get_double("audio.vad_threshold", 0.5); }
int Config::mic_device_index() const { return get_int("audio.mic_device_index", -1); }

int Config::camera_device() const { return get_int("camera.device_index", 0); }
int Config::camera_width() const { return get_int("camera.width", 640); }
int Config::camera_height() const { return get_int("camera.height", 480); }
int Config::camera_fps() const { return get_int("camera.fps", 30); }

double Config::face_match_threshold() const { return get_double("face_recognition.match_threshold", 0.6); }
int Config::face_enrollment_samples() const { return get_int("face_recognition.enrollment_samples", 5); }

std::string Config::cloud_api_key_env() const { return get("llm.cloud_api_key_env", "OPENAI_API_KEY"); }

std::string Config::cloud_api_key() const {
    std::string secrets_path = (fs::path(config_dir_) / "secrets.yaml").string();
    if (fs::exists(secrets_path)) {
        try {
            YAML::Node root = YAML::LoadFile(secrets_path);
            if (root["openai_api_key"] && root["openai_api_key"].IsScalar()) {
                std::string key = root["openai_api_key"].Scalar();
                if (!key.empty() && key != "sk-your-openai-api-key-here") {
                    return key;
                }
            }
        } catch (...) {}
    }
    const char* env_key = std::getenv(cloud_api_key_env().c_str());
    return env_key ? std::string(env_key) : "";
}
int Config::local_context_size() const { return get_int("llm.local_context_size", 4096); }
std::string Config::cloud_model() const { return get("llm.cloud_model", "gpt-4o-mini"); }
int Config::route_to_cloud_tokens() const { return get_int("llm.route_to_cloud_tokens", 2000); }

std::string Config::user_name() const { return get("personality.user_name", "sir"); }

std::string Config::get(const std::string& key, const std::string& default_val) const {
    auto it = kv_.find(key);
    return it != kv_.end() ? it->second : default_val;
}

int Config::get_int(const std::string& key, int default_val) const {
    auto s = get(key, "");
    if (s.empty()) return default_val;
    try {
        return std::stoi(s);
    } catch (...) {
        return default_val;
    }
}

double Config::get_double(const std::string& key, double default_val) const {
    auto s = get(key, "");
    if (s.empty()) return default_val;
    try {
        return std::stod(s);
    } catch (...) {
        return default_val;
    }
}

}  // namespace core
}  // namespace jarvis
