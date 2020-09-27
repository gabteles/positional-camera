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

// Frame Rater
class FrameRater {
public:
  FrameRater(int fps) :
    timeBetweenFrames(1000000 / fps),
    tp { chrono::steady_clock::now() }
    {}

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


// Face Detection
int faceDirectionScore(cv::Mat frame);
void debugFaceDetection(cv::Mat frame);

// Source Ctl
class IVideoSource {
public:
  virtual Mat getFrame() = 0;
  void startCapturing() {
    keepCapturing = true;

    this->captureThread = new thread([this]() {
      while (this->keepCapturing)
        this->capture();
    });
  };
  void stopCapturing() { keepCapturing = false; };

protected:
  virtual void capture() = 0;

private:
  bool keepCapturing;
  thread *captureThread;
};

class VideoSource : public IVideoSource {
public:
  VideoSource(string videoId);
  void connectSource();
  void rejectPastFrames();
  void readFrame();
  virtual Mat getFrame();
  int getFaceDirectionScore();
  int getFps();

protected:
  virtual void capture();

private:
  string name;
  VideoCapture cap;
  Mat frame;
  std::mutex *mutex;
  int frontalFaceArea;
  int fps;
  FrameRater *frameRater;
};

class SourceSelector : public IVideoSource {
public:
  SourceSelector(vector<VideoSource*> *sources);
  virtual Mat getFrame();
  virtual void startCapturing();
  virtual void stopCapturing();

protected:
  virtual void capture();

private:
  void resetRound();
  void initializeFrameRater();
  void evaluateSourceScores();
  int getRoundWinner();
  void processRound();
  string formatResult(vector<int> scores);

  VideoSource* selectedSource;
  vector<VideoSource*> *sources;
  vector<int> sourceScores;
  vector<int> roundResults;
  FrameRater *frameRater;
};

// Output Ctl
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

#endif
