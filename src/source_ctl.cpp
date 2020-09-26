#include <camera_ctl.hpp>

VideoSource::VideoSource(string videoId) {
  this->name = videoId;
  this->mutex = new std::mutex();
  this->frame = Mat(OP_HEIGHT, OP_WIDTH, CV_8UC3);
  this->connectSource();
}

void VideoSource::connectSource() {
  this->cap = VideoCapture(this->name);
  this->cap.set(CAP_PROP_FRAME_WIDTH, OP_WIDTH);
  this->cap.set(CAP_PROP_FRAME_HEIGHT, OP_HEIGHT);
  this->fps = this->cap.get(CAP_PROP_FPS);
  this->frameRater = new FrameRater(this->fps);
  cout << "[SOURCE] Desired FPS: " << this->fps << endl;
}

void VideoSource::rejectPastFrames() {
  using clock = chrono::system_clock;
  auto startTime = clock::now();
  double currentFps = 0;
  bool grabbed = true;

  while (currentFps <= this->fps && grabbed) {
    currentFps = (clock::now() - startTime).count() * (1.0f / this->fps);
    grabbed = this->cap.grab();
  }
}

int VideoSource::getFps() {
  return fps;
}

Mat VideoSource::getFrame() {
  return frame;
}

void VideoSource::readFrame() {
  this->rejectPastFrames();

  Mat frame;
  bool grabbed = this->cap.retrieve(frame);

  if (grabbed) {
    this->mutex->lock();
    resize(frame, this->frame, Size(OP_WIDTH, OP_HEIGHT));
    this->mutex->unlock();
  }
}

int VideoSource::getFaceDirectionScore() {
  mutex->lock();
  int score = faceDirectionScore(frame);
  mutex->unlock();

  return score;
}

void VideoSource::readFrameLoop() {
  while (true) {
    frameRater->sleep();

    if (this->cap.isOpened()) {
      this->readFrame();
    } else {
      cout << "[SOURCE] Camera source not connected (" << this->name << "). Reconnecting... " << endl;
      this->connectSource();
      this_thread::sleep_for(chrono::milliseconds(OP_COMPARE_MS * 10));
    }
  }
}
