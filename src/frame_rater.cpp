#include <camera_ctl.hpp>

FrameRater::FrameRater(int fps) {
  this->timeBetweenFrames = chrono::duration<double, micro>(1000000 / fps);
  this->tp = chrono::steady_clock::now();
}

void FrameRater::sleep() {
  // add to time point
  tp += timeBetweenFrames;

  // and sleep until that time point
  this_thread::sleep_until(tp);
}
