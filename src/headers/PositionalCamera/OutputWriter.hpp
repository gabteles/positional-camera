#ifndef __POSITIONAL_CAMERA_OUTPUT_WRITER__
#define __POSITIONAL_CAMERA_OUTPUT_WRITER__

namespace PositionalCamera {
  class OutputWriter {
  public:
    OutputWriter(string output, int width, int height, int fps, IVideoSource *source);
    void outputLoop();

  private:
    void setupOutput(int width, int height);
    void outputFrame();

    int outputFd;
    FrameRater *frameRater;
    IVideoSource *source;
  };
};

#endif
