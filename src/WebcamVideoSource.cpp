#include <PositionalCamera.hpp>

namespace PositionalCamera {
  WebcamVideoSource::WebcamVideoSource(string videoId) {
    this->name = videoId;
    this->frame = Mat();
    this->connectSource();
  }

  void WebcamVideoSource::connectSource() {
    this->cap = VideoCapture(this->name);
    this->cap.release();
    this->cap.open(this->name);
    if (this->cap.isOpened()) {
      cout << "[SOURCE] Connected to " << this->name << endl;
      this->fps = (int)this->cap.get(CAP_PROP_FPS);
      this->frameRater = new FrameRater(this->fps);
      cout << "[SOURCE] Desired FPS: " << this->fps << endl;
    }
  }

  bool WebcamVideoSource::rejectPastFrames() {
    using clock = chrono::system_clock;
    auto startTime = clock::now();
    double currentFps = 0;
    bool grabbed = true;

    while (currentFps <= this->fps && grabbed) {
      currentFps = (clock::now() - startTime).count() * (1.0f / this->fps);
      grabbed = this->cap.grab();
    }

    return grabbed;
  }

  int WebcamVideoSource::getFps() {
    return fps;
  }

  void WebcamVideoSource::readFrame() {
    bool stillOpen = this->rejectPastFrames();

    if (!stillOpen) {
      this->cap.release();
      return;
    }

    Mat srcFrame;
    this->cap.retrieve(srcFrame);
    this->replaceFrame(srcFrame);
  }

  void WebcamVideoSource::capture() {
    if (this->cap.isOpened()) {
      this->frameRater->sleep();
      this->readFrame();
    } else {
      cout << "[SOURCE] Camera source not connected (" << this->name << "). Reconnecting... " << endl;
      this_thread::sleep_for(chrono::milliseconds(1000));
      this->connectSource();
    }
  }
};
