#include <camera_ctl.hpp>

void connectSource(captureSource* cam) {
  cam->cap = cv::VideoCapture(cam->name);
  cam->cap.set(cv::CAP_PROP_FRAME_WIDTH, OP_WIDTH);
  cam->cap.set(cv::CAP_PROP_FRAME_HEIGHT, OP_HEIGHT);
}

captureSource *setupCameraSource(const char *videoId) {
  std::cout << "[SOURCE] Camera source: " << videoId << std::endl;
  captureSource *source = new captureSource;
  source->name = videoId;
  source->mutex = new std::mutex();
  connectSource(source);
  source->frame = cv::Mat(OP_HEIGHT, OP_WIDTH, CV_8UC3);
  source->roundsWon = 0;
  return source;
}

void rejectPastFrames(captureSource *cam) {
  using clock = std::chrono::system_clock;
  auto startTime = clock::now();
  double desiredFps = (1.0f / OP_FPS);
  double currentFps = 0;

  while (currentFps < desiredFps) {
    currentFps = (clock::now() - startTime).count() * cam->cap.get(cv::CAP_PROP_FPS);
    cam->cap.grab();
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
  while (true) {
    if (cam->cap.isOpened()) {
      readFrame(cam);
    } else {
      std::cout << "[SOURCE] Camera source not connected (" << cam->name << "). Reconnecting... " << std::endl;
      connectSource(cam);
      std::this_thread::sleep_for(std::chrono::milliseconds(OP_COMPARE_MS * 10));
    }
  }
}
