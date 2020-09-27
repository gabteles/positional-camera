#ifndef __POSITIONAL_CAMERA_WEBCAM_VIDEO_SOURCE__
#define __POSITIONAL_CAMERA_WEBCAM_VIDEO_SOURCE__

namespace PositionalCamera {
  class WebcamVideoSource : public IVideoSource {
  public:
    WebcamVideoSource(string videoId);
    int getFps();

  protected:
    virtual void capture();
    void readFrame();
    void connectSource();
    void rejectPastFrames();

  private:
    string name;
    VideoCapture cap;
    int frontalFaceArea;
    int fps;
    FrameRater *frameRater;
  };
};

#endif
