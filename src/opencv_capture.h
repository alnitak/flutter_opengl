#ifndef OPENCV_CAPTURE_H
#define OPENCV_CAPTURE_H

#include "Sampler2D.h"

#include <vector>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>

enum CameraMsg {
    MSG_CAMERA_NONE,
    MSG_CAMERA_STOP,
    MSG_CAMERA_GET_MAT_FRAME
};

class OpenCVCapture {

public:
    OpenCVCapture();
    ~OpenCVCapture();
    bool open(const std::string& uniformName,
              const std::string& completeFilePath,
              int *width, int *height);
    cv::Mat getNewFrame();
    void start(Sampler2D *sampler);
    void stop();
    cv::Mat getCurrentMatFrame();
    inline bool isCameraThreadRunning() { return cameraThreadRunning; }
    inline std::string getUniformName() { return uniformName; }
    std::atomic<CameraMsg> message;

private:
    cv::Mat frame;
    bool cameraThreadRunning;
    cv::VideoCapture cap;
    int width;
    int height;
    std::string uniformName;
    double fps;
    double frameDuration;
};

#endif //OPENCV_CAPTURE_H