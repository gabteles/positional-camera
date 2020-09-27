#include <PositionalCamera.hpp>

namespace PositionalCamera {
  CascadeClassifier FrontalFaceClassifier("./haarcascade_frontalface_default.xml");
  CascadeClassifier ProfileFaceClassifier("./haarcascade_profileface.xml");

  int FaceDetector::evaluate(Mat frame) {
    FrameFaces faces = identifyFaces(frame);
    int frontalConfidence = getLargestFaceConfidence(faces.frontal);
    int profileConfidence = getLargestFaceConfidence(faces.profile);

    return frontalConfidence - profileConfidence;
  }

  FrameFaces FaceDetector::identifyFaces(Mat frame) {
    FrameFaces faces;

    Mat gray;
    cvtColor(frame, gray, CV_BGR2GRAY);

    detectFaces(gray, &faces.frontal, FrontalFaceClassifier);
    detectFaces(gray, &faces.profile, ProfileFaceClassifier);

    return faces;
  }

  void FaceDetector::detectFaces(Mat frame, vector<DetectedFace> *faces, CascadeClassifier classifier) {
    vector<Rect> faceRects;
    vector<int> numObjects;
    classifier.detectMultiScale(frame, faceRects, numObjects);

    for (int i = 0; i < faceRects.size(); i++) {
      Rect faceRect = faceRects.at(i);
      DetectedFace faceEl;
      faceEl.outline = faceRect;
      faceEl.confidence = numObjects.at(i);
      faces->push_back(faceEl);
    }
  }

  int FaceDetector::getLargestFaceConfidence(vector<DetectedFace> faces) {
    int index = faces.size() - 1;

    for (int i = 0; i < faces.size(); i++) {
      if (faces.at(i).outline.area() > faces.at(index).outline.area()) {
        index = i;
      }
    }

    return (index >= 0 ? faces.at(index).confidence : 9);
  }
};
