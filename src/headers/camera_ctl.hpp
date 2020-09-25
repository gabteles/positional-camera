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

#ifndef CAMERA_CTL_FACE_DETECTION
#define CAMERA_CTL_FACE_DETECTION

using namespace std;
using namespace cv;

#define OP_WIDTH 640
#define OP_HEIGHT 480
#define OP_FPS 30
#define OP_ROUNDS 7
#define OP_COMPARE_MS 500

typedef struct capSource {
  const char* name;
  cv::VideoCapture cap;
  cv::Mat frame;
  std::mutex* mutex;
  int frontalFaceArea;
  int fps;
} captureSource;

// Face Detection
int faceDirectionScore(cv::Mat frame);
void debugFaceDetection(cv::Mat frame);

// Source Ctl
captureSource *setupCameraSource(const char *videoId);
void readFrameLoop(captureSource *cam);

// Output Ctl
class OutputController {
public:
  OutputController(std::string output, int width, int height);
  void switchSource(captureSource *source);
  void outputLoop();

private:
  void outputFrame();

  int outputFd;
  captureSource *outputSource;
  std::mutex outputMutex;
};

// Round Ctl
void compareFrames(vector<captureSource *> cams, OutputController* output);

class FrameRater {
public:
  FrameRater(int fps) : timeBetweenFrames(1000000 / fps) , tp { chrono::steady_clock::now() } {

  }

  void sleep() {
    // add to time point
    tp += timeBetweenFrames;

    // and sleep until that time point
    this_thread::sleep_until(tp);
  }

private:
    // a duration with a length of 1/FPS seconds
  chrono::duration<double, micro> timeBetweenFrames;

  // the time point we'll add to in every loop
  chrono::time_point<chrono::steady_clock, decltype(timeBetweenFrames)> tp;
};

#endif
