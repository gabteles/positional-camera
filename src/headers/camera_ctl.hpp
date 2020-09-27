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

#ifndef CAMERA_CTL_FACE_DETECTION
#define CAMERA_CTL_FACE_DETECTION

using namespace std;
using namespace cv;

#define OP_WIDTH 640
#define OP_HEIGHT 480
#define OP_FPS 30
#define OP_ROUNDS 7
#define OP_COMPARE_MS 500

// Frame Rater
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


// Face Detection
typedef struct {
  Rect outline;
  int confidence;
} DetectedFace;

typedef struct {
  vector<DetectedFace> profile;
  vector<DetectedFace> frontal;
} FrameFaces;

class FaceDetector {
public:
  static int evaluate(Mat frame);

private:
  FaceDetector() {}
  static FrameFaces identifyFaces(Mat frame);
  static int getLargestFaceConfidence(vector<DetectedFace> faces);
  static void detectFaces(Mat frame, vector<DetectedFace> *faces, CascadeClassifier classifier);
};

// Source Ctl
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
  void executeWithFrame(std::function<void(Mat)> fn) {
    this->frameMutex.lock();
    fn(this->getFrame());
    this->frameMutex.unlock();
  }
  virtual Mat getFrame() { return this->frame; }

protected:
  virtual void capture() = 0;
  Mat frame;

private:
  bool keepCapturing;
  thread *captureThread;
  std::mutex frameMutex;
};

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

class SourceSelector : public IVideoSource {
public:
  SourceSelector(vector<IVideoSource*> *sources);
  virtual void startCapturing();
  virtual void stopCapturing();

protected:
  virtual Mat getFrame();
  virtual void capture();

private:
  void resetRound();
  void initializeFrameRater();
  void evaluateSourceScores();
  int getRoundWinner();
  void processRound();
  string formatResult(vector<int> scores);

  IVideoSource* selectedSource;
  vector<IVideoSource*> *sources;
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
