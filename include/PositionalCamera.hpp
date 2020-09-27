#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iterator>
#include <functional>

#ifndef CAMERA_CTL_HPP
#define CAMERA_CTL_HPP

namespace PositionalCamera {
  using namespace std;
  using namespace cv;

  class FrameRater;
  class FaceDetector;
  class IVideoSource;
  class WebcamVideoSource;
  class SourceSelector;
  class OutputWriter;
}

#include <PositionalCamera/FrameRater.hpp>
#include <PositionalCamera/FaceDetector.hpp>
#include <PositionalCamera/IVideoSource.hpp>
#include <PositionalCamera/WebcamVideoSource.hpp>
#include <PositionalCamera/SourceSelector.hpp>
#include <PositionalCamera/OutputWriter.hpp>

#endif
