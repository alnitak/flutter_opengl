#ifndef OPENCV_CAMERA_H
#define OPENCV_CAMERA_H

#include <vector>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video/video.hpp>

enum CameraMsg {
    MSG_NONE,
    MSG_STOP,
    MSG_GET_MAT_FRAME
};

class OpenCVCamera {

public:
    OpenCVCamera();
    void open(int width, int height);
    cv::Mat getNewFrame();
    void start(GdkGLContext* context,
            unsigned int texture_name,
            g_autoptr(FlTexture) texture,
            FlTextureRegistrar* texture_registrar
            );
    void stop();
    cv::Mat getCurrentMatFrame();
    bool isCameraThredRunning() {return cameraThredRunning;}
    std::atomic<CameraMsg> message;

private:
    cv::Mat frame;
    bool cameraThredRunning;
    cv::VideoCapture cap;
    int width;
    int height;
};

#endif //OPENCV_CAMERA_H