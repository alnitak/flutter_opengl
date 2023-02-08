//0 QSize(640, 480) 30 - 30 Format_YUYV
//1 QSize(160, 120) 30 - 30 Format_YUYV
//2 QSize(176, 144) 30 - 30 Format_YUYV
//3 QSize(320, 176) 30 - 30 Format_YUYV
//4 QSize(320, 240) 30 - 30 Format_YUYV
//5 QSize(432, 240) 30 - 30 Format_YUYV
//6 QSize(352, 288) 30 - 30 Format_YUYV
//7 QSize(544, 288) 30 - 30 Format_YUYV
//8 QSize(640, 360) 30 - 30 Format_YUYV
//9 QSize(752, 416) 24 - 24 Format_YUYV
//10 QSize(800, 448) 24 - 24 Format_YUYV
//11 QSize(864, 480) 20 - 20 Format_YUYV
//12 QSize(960, 544) 20 - 20 Format_YUYV
//13 QSize(1024, 576) 10 - 10 Format_YUYV
//14 QSize(800, 600) 20 - 20 Format_YUYV
//15 QSize(1184, 656) 10 - 10 Format_YUYV
//16 QSize(960, 720) 15 - 15 Format_YUYV
//17 QSize(1280, 720) 10 - 10 Format_YUYV
//18 QSize(1392, 768) 7.5 - 7.5 Format_YUYV
//19 QSize(1504, 832) 5 - 5 Format_YUYV
//20 QSize(1600, 896) 5 - 5 Format_YUYV
//21 QSize(1280, 960) 5 - 5 Format_YUYV
//22 QSize(1712, 960) 5 - 5 Format_YUYV
//23 QSize(1792, 1008) 5 - 5 Format_YUYV
//24 QSize(1920, 1080) 2 - 2 Format_YUYV
//25 QSize(1600, 1200) 2 - 2 Format_YUYV
//26 QSize(2048, 1536) 2 - 2 Format_YUYV
//27 QSize(2592, 1944) 2 - 2 Format_YUYV
//28 QSize(640, 480) 60 - 60 Format_Jpeg
//29 QSize(160, 120) 30 - 30 Format_Jpeg
//30 QSize(176, 144) 30 - 30 Format_Jpeg
//31 QSize(320, 176) 30 - 30 Format_Jpeg
//32 QSize(320, 240) 60 - 60 Format_Jpeg
//33 QSize(432, 240) 30 - 30 Format_Jpeg
//34 QSize(352, 288) 30 - 30 Format_Jpeg
//35 QSize(544, 288) 30 - 30 Format_Jpeg
//36 QSize(640, 360) 60 - 60 Format_Jpeg
//37 QSize(752, 416) 30 - 30 Format_Jpeg
//38 QSize(800, 448) 30 - 30 Format_Jpeg
//39 QSize(864, 480) 30 - 30 Format_Jpeg
//40 QSize(960, 544) 30 - 30 Format_Jpeg
//41 QSize(1024, 576) 30 - 30 Format_Jpeg
//42 QSize(800, 600) 30 - 30 Format_Jpeg
//43 QSize(1184, 656) 30 - 30 Format_Jpeg
//44 QSize(960, 720) 30 - 30 Format_Jpeg
//45 QSize(1280, 720) 30 - 30 Format_Jpeg
//46 QSize(1392, 768) 15 - 15 Format_Jpeg
//47 QSize(1504, 832) 15 - 15 Format_Jpeg
//48 QSize(1600, 896) 15 - 15 Format_Jpeg
//49 QSize(1280, 960) 15 - 15 Format_Jpeg
//50 QSize(1712, 960) 15 - 15 Format_Jpeg
//51 QSize(1792, 1008) 15 - 15 Format_Jpeg
//52 QSize(1920, 1080) 30 - 30 Format_Jpeg
//53 QSize(1600, 1200) 15 - 15 Format_Jpeg
//54 QSize(2048, 1536) 15 - 15 Format_Jpeg
//55 QSize(2592, 1944) 10 - 10 Format_Jpeg

#ifdef __linux__
#include <flutter_linux/flutter_linux.h>
#include <EGL/egl.h>
#endif
#include "gl/fl_my_texture_gl.h"
#include "opencv_camera.h"
#include <GL/gl.h>
#include <thread>

OpenCVCamera::OpenCVCamera() 
    : cameraThredRunning(false),
      message(MSG_NONE) {
}

void OpenCVCamera::open(int width, int height) {
    if (cap.isOpened()) {
      std::cerr << "Camera already opened!" << std::endl;
      return;
    }
    cap.open(0);
    if (!cap.isOpened()) {
      std::cerr << "ERROR! Unable to open camera\n" << std::endl;
      return;
    }
    this->width = width;
    this->height = height;
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
}

cv::Mat OpenCVCamera::getNewFrame() {
    cap.read(frame);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    return frame;
}

void OpenCVCamera::stop() {
  if (!cameraThredRunning) {
      std::cerr << "ERROR! Camera thread already stopped!" << std::endl;
      return;
  }
  message = MSG_STOP;
}

void OpenCVCamera::start(GdkGLContext* context,
            unsigned int texture_name,
            g_autoptr(FlTexture) texture,
            FlTextureRegistrar* texture_registrar
            ) {

if (cameraThredRunning) {
    std::cerr << "ERROR! Camera thread already running!" << std::endl;
    return;
}
cameraThredRunning = true;

std::thread camera_thread([](OpenCVCamera *me,
            GdkGLContext* context,
            unsigned int texture_name,
            g_autoptr(FlTexture) texture,
            FlTextureRegistrar* texture_registrar) {
    // cv::Mat frame;
    for (;;) {
      if (me->message == MSG_STOP) {
        me->message = MSG_NONE;
        me->cameraThredRunning = false;
        return;
      }

      gdk_gl_context_make_current(context);

      me->cap.read(me->frame);
      // if (me->frame.empty()) continue;
      // if (me->message == MSG_GET_MAT_FRAME) {
      //   me->currentFrame = me->frame;
      //   me->message = MSG_NONE;
      // }

      std::vector<uint8_t> buffer;
      if (me->frame.isContinuous()) {
          buffer.assign(me->frame.data, me->frame.data + me->frame.total() * me->frame.channels());
      } else {
          for (int i = 0; i < me->frame.rows; ++i) {
              buffer.insert(
                buffer.end(), 
                me->frame.ptr<uchar>(i), 
                me->frame.ptr<uchar>(i) + me->frame.cols * me->frame.channels());
          }
      }

      glBindTexture(GL_TEXTURE_2D, texture_name); 
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, me->width, me->height, 
              0, GL_BGR, GL_UNSIGNED_BYTE, buffer.data());
      fl_texture_registrar_mark_texture_frame_available(texture_registrar, texture);
      gdk_gl_context_clear_current();
    }
  },
    this,
    context,
    texture_name,
    texture,
    texture_registrar);
    
  camera_thread.detach();

}

cv::Mat OpenCVCamera::getCurrentMatFrame() {
  if (!cameraThredRunning || message != MSG_NONE) return cv::Mat();
  // std::thread getFrame_thread([](OpenCVCamera *me) {
  //   me->message = MSG_GET_MAT_FRAME;
  //   while (me->message != MSG_NONE);
  // }, this);
  // getFrame_thread.join();
  // return currentFrame;
  return frame;
}