#pragma once

#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include "vision/face_recognition_net.h"
#include <opencv2/core.hpp>
#include <vector>
#include <string>
#include <memory>

namespace jarvis {
namespace vision {

class FaceRecognition {
public:
    FaceRecognition(const std::string& landmarks_path,
                    const std::string& face_db_dir,
                    double match_threshold = 0.6);

    bool load();
    void enroll_face(const std::string& label, const std::vector<cv::Mat>& samples);
    bool recognize_face(const cv::Mat& frame, std::string& out_label);
    bool detect_face(const cv::Mat& frame, cv::Rect& out_rect);
    bool has_enrolled_faces() const { return !known_faces_.empty(); }
    dlib::matrix<float, 0, 1> get_face_descriptor(const cv::Mat& frame, const cv::Rect& rect);

private:
    dlib::matrix<dlib::rgb_pixel> cvmat_to_dlib(const cv::Mat& mat);
    dlib::frontal_face_detector detector_;
    dlib::shape_predictor sp_;
    std::string landmarks_path_;
    std::string face_db_dir_;
    double match_threshold_;
    std::vector<std::pair<std::string, dlib::matrix<float, 0, 1>>> known_faces_;
    std::unique_ptr<detail::anet_type> net_;  // ResNet for 128-d embeddings
    bool use_resnet_{false};
    bool loaded_{false};
};

}  // namespace vision
}  // namespace jarvis
