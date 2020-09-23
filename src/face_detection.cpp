#include <camera_ctl.hpp>

typedef struct {
  cv::Rect outline;
  int confidence;
} detectedFace;

typedef struct {
  std::vector<detectedFace> profile;
  std::vector<detectedFace> frontal;
} frameFaces;

cv::CascadeClassifier frontalFaceClassifier("./haarcascade_frontalface_default.xml");
cv::CascadeClassifier profileFaceClassifier("./haarcascade_profileface.xml");

void detectFaces(cv::Mat frame, std::vector<detectedFace> *faces, cv::CascadeClassifier classifier) {
  std::vector<cv::Rect> faceRects;
  std::vector<int> numObjects;
  classifier.detectMultiScale(frame, faceRects, numObjects);

  for (int i = 0; i < faceRects.size(); i++) {
    cv::Rect faceRect = faceRects.at(i);
    detectedFace faceEl;
    faceEl.outline = faceRect;
    faceEl.confidence = numObjects.at(i);
    faces->push_back(faceEl);
  }
}

frameFaces identifyFrameFaces(cv::Mat frame) {
  frameFaces faces;

  cv::Mat gray;
  cv::cvtColor(frame, gray, CV_BGR2GRAY);

  detectFaces(gray, &faces.frontal, frontalFaceClassifier);
  detectFaces(gray, &faces.profile, profileFaceClassifier);

  return faces;
}

int getLargestFaceConfidence(std::vector<detectedFace> faces) {
  int index = faces.size() - 1;

  for (int i = 0; i < faces.size(); i++) {
    if (faces.at(i).outline.area() > faces.at(index).outline.area()) {
      index = i;
    }
  }

  return (index >= 0 ? faces.at(index).confidence : 9);
}

int faceDirectionScore(cv::Mat frame) {
  frameFaces faces = identifyFrameFaces(frame);
  int frontalConfidence = getLargestFaceConfidence(faces.frontal);
  int profileConfidence = getLargestFaceConfidence(faces.profile);

  return frontalConfidence - profileConfidence;
}

void debugFaceDetection(cv::Mat frame) {
  frameFaces faces = identifyFrameFaces(frame);

  for (auto face : faces.frontal) {
    cv::rectangle(frame, face.outline, cv::Scalar(255, 0, 0));
    cv::putText(frame, std::to_string(face.confidence), cvPoint(face.outline.x, face.outline.y + 10), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255, 0, 0));
  }

  for (auto face : faces.profile) {
    cv::rectangle(frame, face.outline, cv::Scalar(0, 0, 255));
    cv::putText(frame, std::to_string(face.confidence), cvPoint(face.outline.x, face.outline.y + 10), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 0, 255));
  }
}
