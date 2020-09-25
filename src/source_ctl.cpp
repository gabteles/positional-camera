#include <camera_ctl.hpp>

void connectSource(captureSource* cam) {
  cam->cap = cv::VideoCapture(cam->name);
  cam->cap.set(cv::CAP_PROP_FRAME_WIDTH, OP_WIDTH);
  cam->cap.set(cv::CAP_PROP_FRAME_HEIGHT, OP_HEIGHT);
  cam->fps = cam->cap.get(cv::CAP_PROP_FPS);
  std::cout << "[SOURCE] Desired FPS: " << cam->fps << std::endl;
}

captureSource *setupCameraSource(const char *videoId) {
  std::cout << "[SOURCE] Camera source: " << videoId << std::endl;
  captureSource *source = new captureSource;
  source->name = videoId;
  source->mutex = new std::mutex();
  connectSource(source);
  source->frame = cv::Mat(OP_HEIGHT, OP_WIDTH, CV_8UC3);
  return source;
}

void rejectPastFrames(captureSource *cam) {
  using clock = std::chrono::system_clock;
  auto startTime = clock::now();
  double desiredFps = cam->fps;
  double currentFps = 0;
  bool grabbed = true;

  while (currentFps <= desiredFps && grabbed) {
    currentFps = (clock::now() - startTime).count() * (1.0f / cam->fps);
    grabbed = cam->cap.grab();
  }
}

void readFrame(captureSource *cam) {
  rejectPastFrames(cam);

  cv::Mat frame;
  bool grabbed = cam->cap.retrieve(frame);

  if (grabbed) {
    cam->mutex->lock();
    cv::resize(frame, cam->frame, cv::Size(OP_WIDTH, OP_HEIGHT));
    cam->mutex->unlock();

    outputFrame(cam);
  }
}

void readFrameLoop(captureSource *cam) {
  FrameRater fr(cam->fps);

  while (true) {
    fr.sleep();

    if (cam->cap.isOpened()) {
      readFrame(cam);
    } else {
      std::cout << "[SOURCE] Camera source not connected (" << cam->name << "). Reconnecting... " << std::endl;
      connectSource(cam);
      std::this_thread::sleep_for(std::chrono::milliseconds(OP_COMPARE_MS * 10));
    }
  }
}
