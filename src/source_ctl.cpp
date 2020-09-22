#include <camera_ctl.hpp>

captureSource *setupCameraSource(const char *videoId) {
  std::cout << "[SETUP] Camera source: " << videoId << std::endl;
  captureSource *source = new captureSource;
  source->name = videoId;
  source->mutex = new std::mutex();
  source->cap = cv::VideoCapture(videoId);
  source->cap.set(cv::CAP_PROP_FRAME_WIDTH, OP_WIDTH);
  source->cap.set(cv::CAP_PROP_FRAME_HEIGHT, OP_HEIGHT);
  source->frame = cv::Mat(OP_HEIGHT, OP_WIDTH, CV_8UC3);
  source->roundsWon = 0;
  return source;
}

void readFrameLoop(captureSource *cam) {
  while (cam->cap.isOpened()) {
    using clock = std::chrono::system_clock;
    while (1) {
      auto startTime = clock::now();
      cam->cap.grab();

      if ((clock::now() - startTime).count() * cam->cap.get(cv::CAP_PROP_FPS) > (1.0f / OP_FPS)) {
        break;
      }
    }

    cv::Mat frame;
    cam->cap.retrieve(frame);

    cam->mutex->lock();
    cv::resize(frame, cam->frame, cv::Size(OP_WIDTH, OP_HEIGHT));
    cam->mutex->unlock();

    outputFrame(cam);
  }
}
