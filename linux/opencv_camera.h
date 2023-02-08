#ifndef OPENCV_CAMERA_H
#define OPENCV_CAMERA_H

#include "../src/Sampler2D.h"

#include <vector>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video/video.hpp>

enum CameraMsg {
    MSG_CAMERA_NONE,
    MSG_CAMERA_STOP,
    MSG_CAMERA_GET_MAT_FRAME
};

class OpenCVCamera {

public:
    OpenCVCamera();
    bool open(std::string uniformName, int width, int height);
    cv::Mat getNewFrame();
    void start(Sampler2D *sampler);
    void stop();
    cv::Mat getCurrentMatFrame();
    inline bool isCameraThredRunning() { return cameraThredRunning; }
    inline std::string getUniformName() { return uniformName; }
    std::atomic<CameraMsg> message;

private:
    cv::Mat frame;
    bool cameraThredRunning;
    cv::VideoCapture cap;
    int width;
    int height;
    std::string uniformName;
};

#endif //OPENCV_CAMERA_H