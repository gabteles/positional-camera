#include <camera_ctl.hpp>

cv::Rect selectLargestFace(std::vector<cv::Rect> faces) {
  cv::Rect largest = faces.at(0);
  for (int i = 1; i < faces.size(); i++)
  {
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

  if (faces.size() == 0)
    return 0;

  cv::Rect faceRect = selectLargestFace(faces);
  cv::rectangle(frame, faceRect, cv::Scalar(255, 0, 0));

  cv::Mat face(gray, faceRect);
  std::vector<cv::Rect> eyes;
  eyeCascade.detectMultiScale(face, eyes);

  int eyesArea = 0;
  for (int j = 0; j < eyes.size(); j++)
  {
    cv::Rect eyeRect = eyes.at(j);
    cv::Rect absEyeRect(eyeRect.x + faceRect.x, eyeRect.y + faceRect.y, eyeRect.width, eyeRect.height);
    cv::rectangle(frame, absEyeRect, cv::Scalar(0, 0, 255));
    eyesArea += eyeRect.area();
  }

  return eyes.size() * (faceRect.area() + eyesArea);
}
