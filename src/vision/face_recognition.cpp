#include "vision/face_recognition.h"
#include "vision/face_recognition_net.h"
#include "core/logger.h"
#include "core/config.h"
#include <dlib/image_io.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
namespace jarvis {
namespace vision {

FaceRecognition::FaceRecognition(const std::string& landmarks_path,
                                 const std::string& face_db_dir,
                                 double match_threshold)
    : landmarks_path_(landmarks_path)
    , face_db_dir_(face_db_dir)
    , match_threshold_(match_threshold) {}

bool FaceRecognition::load() {
    try {
        if (!fs::exists(landmarks_path_)) {
            JARVIS_LOG_WARN("Face landmarks file not found: ", landmarks_path_,
                ". Face recognition disabled. Run setup.ps1 to download.");
            return false;
        }
        dlib::deserialize(landmarks_path_) >> sp_;
        detector_ = dlib::get_frontal_face_detector();

        // Load face recognition resnet (optional - for 128-d embeddings)
        std::string resnet_path = fs::path(landmarks_path_).parent_path().string() + "/dlib_face_recognition_resnet_model_v1.dat";
        if (fs::exists(resnet_path)) {
            net_ = std::make_unique<detail::anet_type>();
            dlib::deserialize(resnet_path) >> *net_;
            use_resnet_ = true;
            JARVIS_LOG_INFO("Face recognition ResNet loaded.");
        } else {
            use_resnet_ = false;
            JARVIS_LOG_WARN("Face recognition ResNet not found. Using landmark-based matching (less accurate).");
        }

        fs::create_directories(face_db_dir_);
        for (const auto& entry : fs::directory_iterator(face_db_dir_)) {
            if (entry.path().extension() == ".dat") {
                std::string label = entry.path().stem().string();
                dlib::matrix<float, 0, 1> face_descriptor;
                dlib::deserialize(entry.path().string()) >> face_descriptor;
                known_faces_.emplace_back(label, face_descriptor);
                JARVIS_LOG_DEBUG("Loaded face: ", label);
            }
        }
        loaded_ = true;
        JARVIS_LOG_INFO("Face recognition loaded. ", known_faces_.size(), " known faces.");
        return true;
    } catch (const std::exception& e) {
        JARVIS_LOG_ERROR("Face recognition load failed: ", e.what());
        return false;
    }
}

bool FaceRecognition::detect_face(const cv::Mat& frame, cv::Rect& out_rect) {
    if (!loaded_) return false;
    try {
        dlib::cv_image<dlib::bgr_pixel> cimg(frame);
        auto faces = detector_(cimg);
        if (faces.empty()) return false;
        auto& r = faces[0];
        out_rect = cv::Rect(r.left(), r.top(), r.width(), r.height());
        return true;
    } catch (const std::exception& e) {
        JARVIS_LOG_DEBUG("Face detection error: ", e.what());
        return false;
    }
}

dlib::matrix<float, 0, 1> FaceRecognition::get_face_descriptor(
    const cv::Mat& frame, const cv::Rect& rect) {
    dlib::matrix<float, 0, 1> desc;
    desc.set_size(128);
    desc = 0;
    try {
        dlib::cv_image<dlib::bgr_pixel> cimg(frame);
        dlib::rectangle drect(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
        auto shape = sp_(cimg, drect);
        dlib::matrix<dlib::rgb_pixel> face_chip;
        dlib::extract_image_chip(cimg, dlib::get_face_chip_details(shape, 150, 0.25), face_chip);

        if (use_resnet_ && net_) {
            desc = (*net_)(face_chip);
        } else {
            // Fallback: use landmark coordinates as crude 128-d (pad/truncate)
            for (long i = 0; i < 68 && i < 64; ++i) {
                desc(i) = static_cast<float>(shape.part(i).x()) / 1000.0f;
            }
            for (long i = 0; i < 68 && i < 64; ++i) {
                desc(64 + i) = static_cast<float>(shape.part(i).y()) / 1000.0f;
            }
        }
    } catch (const std::exception& e) {
        JARVIS_LOG_DEBUG("Face descriptor error: ", e.what());
    }
    return desc;
}

void FaceRecognition::enroll_face(const std::string& label,
                                  const std::vector<cv::Mat>& samples) {
    if (!loaded_ || samples.empty()) return;
    std::vector<dlib::matrix<float, 0, 1>> descriptors;
    for (const auto& mat : samples) {
        cv::Rect rect;
        if (!detect_face(mat, rect)) continue;
        auto desc = get_face_descriptor(mat, rect);
        descriptors.push_back(desc);
    }
    if (descriptors.empty()) return;
    dlib::matrix<float, 0, 1> mean_desc;
    mean_desc.set_size(128);
    mean_desc = 0;
    for (const auto& d : descriptors) mean_desc += d;
    mean_desc /= descriptors.size();
    std::string path = face_db_dir_ + "/" + label + ".dat";
    dlib::serialize(path) << mean_desc;
    known_faces_.emplace_back(label, mean_desc);
    JARVIS_LOG_INFO("Enrolled face: ", label);
}

bool FaceRecognition::recognize_face(const cv::Mat& frame, std::string& out_label) {
    if (!loaded_ || known_faces_.empty()) return false;
    cv::Rect rect;
    if (!detect_face(frame, rect)) return false;
    auto desc = get_face_descriptor(frame, rect);
    double best_dist = 1e9;
    std::string best_label;
    for (const auto& [label, known] : known_faces_) {
        double d = dlib::length(desc - known);
        if (d < best_dist && d < match_threshold_) {
            best_dist = d;
            best_label = label;
        }
    }
    if (!best_label.empty()) {
        out_label = best_label;
        return true;
    }
    return false;
}

}  // namespace vision
}  // namespace jarvis
