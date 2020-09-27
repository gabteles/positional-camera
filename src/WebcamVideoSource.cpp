#include <PositionalCamera.hpp>

namespace PositionalCamera {
  WebcamVideoSource::WebcamVideoSource(string videoId) {
    this->name = videoId;
    this->frame = Mat(OP_HEIGHT, OP_WIDTH, CV_8UC3);
    this->connectSource();
  }

  void WebcamVideoSource::connectSource() {
    this->cap = VideoCapture(this->name);
    this->cap.set(CAP_PROP_FRAME_WIDTH, OP_WIDTH);
    this->cap.set(CAP_PROP_FRAME_HEIGHT, OP_HEIGHT);
    this->fps = this->cap.get(CAP_PROP_FPS);
    this->frameRater = new FrameRater(this->fps);
    cout << "[SOURCE] Desired FPS: " << this->fps << endl;
  }

  void WebcamVideoSource::rejectPastFrames() {
    using clock = chrono::system_clock;
    auto startTime = clock::now();
    double currentFps = 0;
    bool grabbed = true;

    while (currentFps <= this->fps && grabbed) {
      currentFps = (clock::now() - startTime).count() * (1.0f / this->fps);
      grabbed = this->cap.grab();
    }
  }

  int WebcamVideoSource::getFps() {
    return fps;
  }

  void WebcamVideoSource::readFrame() {
    this->rejectPastFrames();

    Mat srcFrame;
    bool grabbed = this->cap.retrieve(srcFrame);

    if (grabbed) {
      this->executeWithFrame([srcFrame](Mat dstFrame) {
        resize(srcFrame, dstFrame, Size(OP_WIDTH, OP_HEIGHT));
      });
    }
  }

  void WebcamVideoSource::capture() {
    this->frameRater->sleep();

    if (this->cap.isOpened()) {
      this->readFrame();
    } else {
      cout << "[SOURCE] Camera source not connected (" << this->name << "). Reconnecting... " << endl;
      this->connectSource();
      this_thread::sleep_for(chrono::milliseconds(OP_COMPARE_MS * 10));
    }
  }
};
