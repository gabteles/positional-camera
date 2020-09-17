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

#define OP_WIDTH 640
#define OP_HEIGHT 480
#define OP_FPS 30
#define OP_MAJORITY_ROUNDS 5
#define OP_COMPARE_MS 500

typedef struct {
  const char* name;
  cv::VideoCapture cap;
  cv::Mat frame;
  std::mutex* mutex;
  int frontalFaceArea;
  int roundsWon;
} captureSource;

std::mutex sourceMutex;
captureSource* lookingSource;

bool keepGrabingFrames = true;

int openLoopback() {
  int deviceFd = open("/dev/video22", O_RDWR);
  int width = OP_WIDTH;
  int height = OP_HEIGHT;
  int channels = 3;
  int total = width * height;

  v4l2_format format;
  format.type                 = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  format.fmt.pix.field        = V4L2_FIELD_NONE;
  format.fmt.pix.pixelformat  = V4L2_PIX_FMT_BGR24;
  format.fmt.pix.width        = width;
  format.fmt.pix.height       = height;
  format.fmt.pix.bytesperline = width * channels;
  format.fmt.pix.sizeimage    = total * channels;

  int result = ioctl(deviceFd, VIDIOC_S_FMT, &format);

  return deviceFd;
}

cv::Rect selectLargestFace(std::vector<cv::Rect> faces) {
  cv::Rect largest = faces.at(0);
  for (int i = 1; i < faces.size(); i++) {
    cv::Rect face = faces.at(i);

    if (face.area() > largest.area())
      largest = face;
  }
  return largest;
}

int identifyFrontalFace(cv::Mat frame) {
  cv::CascadeClassifier faceCascade("./haarcascade_frontalface_default.xml");
  cv::CascadeClassifier eyeCascade("./haarcascade_eye.xml");
  cv::Mat gray;

  cv::cvtColor(frame, gray, CV_BGR2GRAY);

  std::vector<cv::Rect> faces;
  faceCascade.detectMultiScale(gray, faces);

  if (faces.size() == 0) return 0;

  cv::Rect faceRect = selectLargestFace(faces);
  cv::rectangle(frame, faceRect, cv::Scalar(255, 0, 0));

  cv::Mat face(gray, faceRect);
  std::vector<cv::Rect> eyes;
  eyeCascade.detectMultiScale(face, eyes);

  int eyesArea = 0;
  for (int j = 0; j < eyes.size(); j++) {
    cv::Rect eyeRect = eyes.at(j);
    cv::Rect absEyeRect(eyeRect.x + faceRect.x, eyeRect.y + faceRect.y, eyeRect.width, eyeRect.height);
    cv::rectangle(frame, absEyeRect, cv::Scalar(0, 0, 255));
    eyesArea += eyeRect.area();
  }

  return eyes.size() * (faceRect.area() + eyesArea);
}

captureSource* setupCameraSource(const char* videoId) {
  std::cout << "[SETUP] Camera source: " << videoId << std::endl;
  captureSource* source = new captureSource;
  source->name = videoId;
  source->mutex = new std::mutex();
  source->cap = cv::VideoCapture(videoId);
  source->cap.set(cv::CAP_PROP_FRAME_WIDTH, OP_WIDTH);
  source->cap.set(cv::CAP_PROP_FRAME_HEIGHT, OP_HEIGHT);
  source->frame = cv::Mat(OP_HEIGHT, OP_WIDTH, CV_8UC3);
  source->roundsWon = 0;
  return source;
}

void readFrameLoop(captureSource *cam, int loopbackFd) {
  while (keepGrabingFrames && cam->cap.isOpened()) {
    using clock = std::chrono::system_clock;
    while (1) {
      auto startTime = clock::now();
      cam->cap.grab();

      if ((clock::now() - startTime).count() * cam->cap.get(cv::CAP_PROP_FPS) > (1.0f / OP_FPS)) {
        break;
      }
    }

    cv::Mat frame;
    cam->cap.retrieve(frame);
    cam->mutex->lock();
    cv::resize(frame, cam->frame, cv::Size(OP_WIDTH, OP_HEIGHT));
    cam->mutex->unlock();

    sourceMutex.lock();
    if (lookingSource == cam) {
      cv::Mat colorCorrectFrame;
      cv::cvtColor(cam->frame, colorCorrectFrame, CV_RGB2BGR);
      write(loopbackFd, colorCorrectFrame.data, colorCorrectFrame.total() * colorCorrectFrame.elemSize());
    }
    sourceMutex.unlock();
  }
}

void switchSource(captureSource* source) {
  sourceMutex.lock();
  lookingSource = source;
  sourceMutex.unlock();
}

void compareFrames(captureSource* cam1, captureSource* cam2) {
  using clock = std::chrono::system_clock;
  while (true) {
    cam1->mutex->lock();
    int faceArea1 = identifyFrontalFace(cam1->frame);
    cam1->mutex->unlock();
    cam2->mutex->lock();
    int faceArea2 = identifyFrontalFace(cam2->frame);
    cam2->mutex->unlock();

    if (faceArea1 == faceArea2) {
      std::cout << "[ROUND] Tie!" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(OP_COMPARE_MS / 100));
      continue;
    }

    if (faceArea1 > faceArea2) {
      cam1->roundsWon++;
    } else {
      cam2->roundsWon++;
    }

    std::cout << "[ROUND] Partial: CAM A - " << cam1->roundsWon << " x " << cam2->roundsWon << " - CAM B" << std::endl;

    if ((cam1->roundsWon + cam2->roundsWon) >= OP_MAJORITY_ROUNDS) {
      if (cam1->roundsWon > cam2->roundsWon) {
        std::cout << "[ROUND] Cam A won round" << std::endl;
        switchSource(cam1);
      } else {
        std::cout << "[ROUND] Cam B won round" << std::endl;
        switchSource(cam2);
      }

      std::cout << "[ROUND] Starting new round" << std::endl;
      cam1->roundsWon = 0;
      cam2->roundsWon = 0;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(OP_COMPARE_MS));
  }
}

int main(int argc, char **argv) {
  captureSource* cam1 = setupCameraSource(argv[1]);
  captureSource* cam2 = setupCameraSource(argv[2]);
  lookingSource = cam2;

  int loopbackFd = openLoopback();

  std::thread cam1Thread(readFrameLoop, cam1, loopbackFd);
  std::thread cam2Thread(readFrameLoop, cam2, loopbackFd);

  compareFrames(cam1, cam2);

  return 0;
}
