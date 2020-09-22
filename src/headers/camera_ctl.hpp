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

#define OP_WIDTH 640
#define OP_HEIGHT 480
#define OP_FPS 30
#define OP_ROUNDS 5
#define OP_COMPARE_MS 500

typedef struct {
  const char* name;
  cv::VideoCapture cap;
  cv::Mat frame;
  std::mutex* mutex;
  int frontalFaceArea;
  int roundsWon;
} captureSource;

// Face Detection
cv::Rect selectLargestFace(std::vector<cv::Rect> faces);
int identifyFrontalFace(cv::Mat frame);

// Source Ctl
captureSource *setupCameraSource(const char *videoId);
void readFrameLoop(captureSource *cam);

// Output Ctl
void openLoopback(char *output);
void outputFrame(captureSource *cam);
void switchSource(captureSource *source);

// Round Ctl
void compareFrames(captureSource *cam1, captureSource *cam2);

#endif
