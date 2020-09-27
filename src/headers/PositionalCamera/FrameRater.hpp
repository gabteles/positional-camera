#ifndef __POSITIONAL_CAMERA_FRAME_RATER__
#define __POSITIONAL_CAMERA_FRAME_RATER__

namespace PositionalCamera {
  class FrameRater {
  public:
    FrameRater(int fps);
    void sleep();

  private:
    // a duration with a length of 1/FPS seconds
    chrono::duration<double, micro> timeBetweenFrames;

    // the time point we'll add to in every loop
    chrono::time_point<chrono::steady_clock, decltype(timeBetweenFrames)> tp;
  };
};

#endif
