// 0 QSize(640, 480) 30 - 30 Format_YUYV
// 1 QSize(160, 120) 30 - 30 Format_YUYV
// 2 QSize(176, 144) 30 - 30 Format_YUYV
// 3 QSize(320, 176) 30 - 30 Format_YUYV
// 4 QSize(320, 240) 30 - 30 Format_YUYV
// 5 QSize(432, 240) 30 - 30 Format_YUYV
// 6 QSize(352, 288) 30 - 30 Format_YUYV
// 7 QSize(544, 288) 30 - 30 Format_YUYV
// 8 QSize(640, 360) 30 - 30 Format_YUYV
// 9 QSize(752, 416) 24 - 24 Format_YUYV
// 10 QSize(800, 448) 24 - 24 Format_YUYV
// 11 QSize(864, 480) 20 - 20 Format_YUYV
// 12 QSize(960, 544) 20 - 20 Format_YUYV
// 13 QSize(1024, 576) 10 - 10 Format_YUYV
// 14 QSize(800, 600) 20 - 20 Format_YUYV
// 15 QSize(1184, 656) 10 - 10 Format_YUYV
// 16 QSize(960, 720) 15 - 15 Format_YUYV
// 17 QSize(1280, 720) 10 - 10 Format_YUYV
// 18 QSize(1392, 768) 7.5 - 7.5 Format_YUYV
// 19 QSize(1504, 832) 5 - 5 Format_YUYV
// 20 QSize(1600, 896) 5 - 5 Format_YUYV
// 21 QSize(1280, 960) 5 - 5 Format_YUYV
// 22 QSize(1712, 960) 5 - 5 Format_YUYV
// 23 QSize(1792, 1008) 5 - 5 Format_YUYV
// 24 QSize(1920, 1080) 2 - 2 Format_YUYV
// 25 QSize(1600, 1200) 2 - 2 Format_YUYV
// 26 QSize(2048, 1536) 2 - 2 Format_YUYV
// 27 QSize(2592, 1944) 2 - 2 Format_YUYV
// 28 QSize(640, 480) 60 - 60 Format_Jpeg
// 29 QSize(160, 120) 30 - 30 Format_Jpeg
// 30 QSize(176, 144) 30 - 30 Format_Jpeg
// 31 QSize(320, 176) 30 - 30 Format_Jpeg
// 32 QSize(320, 240) 60 - 60 Format_Jpeg
// 33 QSize(432, 240) 30 - 30 Format_Jpeg
// 34 QSize(352, 288) 30 - 30 Format_Jpeg
// 35 QSize(544, 288) 30 - 30 Format_Jpeg
// 36 QSize(640, 360) 60 - 60 Format_Jpeg
// 37 QSize(752, 416) 30 - 30 Format_Jpeg
// 38 QSize(800, 448) 30 - 30 Format_Jpeg
// 39 QSize(864, 480) 30 - 30 Format_Jpeg
// 40 QSize(960, 544) 30 - 30 Format_Jpeg
// 41 QSize(1024, 576) 30 - 30 Format_Jpeg
// 42 QSize(800, 600) 30 - 30 Format_Jpeg
// 43 QSize(1184, 656) 30 - 30 Format_Jpeg
// 44 QSize(960, 720) 30 - 30 Format_Jpeg
// 45 QSize(1280, 720) 30 - 30 Format_Jpeg
// 46 QSize(1392, 768) 15 - 15 Format_Jpeg
// 47 QSize(1504, 832) 15 - 15 Format_Jpeg
// 48 QSize(1600, 896) 15 - 15 Format_Jpeg
// 49 QSize(1280, 960) 15 - 15 Format_Jpeg
// 50 QSize(1712, 960) 15 - 15 Format_Jpeg
// 51 QSize(1792, 1008) 15 - 15 Format_Jpeg
// 52 QSize(1920, 1080) 30 - 30 Format_Jpeg
// 53 QSize(1600, 1200) 15 - 15 Format_Jpeg
// 54 QSize(2048, 1536) 15 - 15 Format_Jpeg
// 55 QSize(2592, 1944) 10 - 10 Format_Jpeg

#include "opencv_capture.h"
#include "common.h"
#include "ffi.h"
#include "Renderer.h"

#include <opencv2/core/utils/logger.hpp>
#include <thread>

#ifdef _IS_ANDROID_
#include "EGL/eglext.h"
#include "GLES3/gl3.h"
#include <unistd.h> // for usleep
#endif
#ifdef _IS_LINUX_
#include <GL/glew.h>
#endif

// for testing open file in android
#include <iostream>
#include <fstream>
#include <utility>

OpenCVCapture::OpenCVCapture()
	: message(MSG_CAMERA_NONE),
      cameraThreadRunning(false)
{
}

OpenCVCapture::~OpenCVCapture()
{
	if (cameraThreadRunning) stop();
}

bool OpenCVCapture::open(const std::string& uniformName,
                        const std::string& completeFilePath,
                        int *width, int *height)
{
    // webCam on Android seems not to work. Probably ffmpeg is needed by OpenCV also
    // to speed up video files

    // https://docs.opencv.org/3.4/d4/d15/group__videoio__flags__base.html
	int apiCamPreference = 0;
	int apiFilePreference = 0;
	#ifdef _IS_ANDROID_
        apiCamPreference = cv::CAP_ANDROID;
	#endif
	#ifdef _IS_LINUX_
        apiCamPreference = cv::CAP_V4L2;
    #endif

	#ifdef _IS_ANDROID_
        apiFilePreference = cv::CAP_ANDROID;
	#endif
	#ifdef _IS_LINUX_
        apiFilePreference = cv::CAP_ANY;
	#endif

    if (completeFilePath == "cam0")
        cap.open(0, apiCamPreference);
    else if (completeFilePath == "cam1")
        cap.open(1, apiCamPreference);
    else
        cap.open(completeFilePath, apiFilePreference);


	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	if (!cap.isOpened())
	{
        LOGD("OCV CAPTURE", "ERROR! Unable to open VideoCapture");
		return false;
	}
    // silence log!
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_ERROR);

    *width = this->width = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
    *height = this->height = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	this->uniformName = std::move(uniformName);
    fps = (int)cap.get(cv::CAP_PROP_FPS);
	frameDuration = 1000000.0 / fps;

	// Set the capture mode to BGR
	// https://docs.opencv.org/3.4/d4/d15/group__videoio__flags__base.html#gad0f42b32af0d89d2cee80dae0ea62b3d
//	 cap.set(cv::CAP_PROP_MODE, 1);
	return true;
}

cv::Mat OpenCVCapture::getNewFrame()
{
	cap.read(frame);
	cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
	return frame;
}

void OpenCVCapture::stop()
{
	if (!cameraThreadRunning)
	{
		LOGD("OCV CAPTURE", "ERROR! Camera thread already stopped!");
		if (cap.isOpened()) cap.release();
		return;
	}
	message = MSG_CAMERA_STOP;
    while (isCameraThreadRunning());
}

void OpenCVCapture::start(Sampler2D *sampler)
{

	if (cameraThreadRunning)
	{
		LOGD("OCV CAPTURE", "ERROR! Camera thread already running!");
		return;
	}
	LOGD("OCV CAPTURE", "Starting camera thread!");

	std::thread camera_thread([](OpenCVCapture *me,
								 Sampler2D *sampler)
							  {
	Renderer *renderer = getRenderer();
	me->cameraThreadRunning = true;
    auto start = std::chrono::steady_clock::now();
	auto end = std::chrono::steady_clock::now();
	int64_t elapsed;

  	for (;;) {
		if (me->message == MSG_CAMERA_STOP) {
			me->cap.release();
			me->cameraThreadRunning = false;
			me->message = MSG_CAMERA_NONE;
            LOGD("OCV CAPTURE", "Stopping camera thread!");
			return;
		}

		me->cap.read(me->frame);
		if (me->frame.empty()) continue;

		#ifdef _IS_ANDROID_
			cv::cvtColor(me->frame, me->frame, cv::COLOR_RGB2RGBA);
        #endif
		#ifdef _IS_LINUX_
			cv::cvtColor(me->frame, me->frame, cv::COLOR_BGR2RGBA);
		#endif
		cv::flip(me->frame, me->frame,0);

 		end = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		if (me->frameDuration > elapsed)
        	usleep(me->frameDuration - elapsed);

		renderer = getRenderer();
		if (renderer != nullptr) 
		{
            if (!me->frame.empty() && elapsed >= me->frameDuration) {
                sampler->replaceTexture(me->width, me->height, me->frame.clone().data );
                getRenderer()->setTextureMsg(*sampler);
				start = std::chrono::steady_clock::now();
            }
		} else
			me->message = MSG_CAMERA_NONE;
	} },
	  this,
	  sampler);

	camera_thread.detach();
}

cv::Mat OpenCVCapture::getCurrentMatFrame()
{
	if (!cameraThreadRunning || message != MSG_CAMERA_NONE)
		return cv::Mat();
	return frame;
}