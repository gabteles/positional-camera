#ifndef __POSITIONAL_CAMERA_I_VIDEO_SOURCE__
#define __POSITIONAL_CAMERA_I_VIDEO_SOURCE__

namespace PositionalCamera {
  class IVideoSource {
  public:
    void startCapturing() {
      keepCapturing = true;

      this->captureThread = new thread([this]() {
        while (this->keepCapturing)
          this->capture();
      });
    };

    void stopCapturing() { keepCapturing = false; };
    void executeWithFrame(function<void(Mat)> fn) {
      this->frameMutex.lock();
      fn(this->getFrame());
      this->frameMutex.unlock();
    }
    void replaceFrame(Mat newFrame) { this->frame = newFrame; }
    virtual Mat getFrame() { return this->frame; }

  protected:
    virtual void capture() = 0;
    Mat frame;

  private:
    bool keepCapturing;
    thread *captureThread;
    std::mutex frameMutex;
  };
};

#endif
