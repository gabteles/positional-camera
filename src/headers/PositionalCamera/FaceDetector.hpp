#ifndef __POSITIONAL_CAMERA_FACE_DETECTOR__
#define __POSITIONAL_CAMERA_FACE_DETECTOR__

namespace PositionalCamera {
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
};

#endif
