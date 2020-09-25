#include <camera_ctl.hpp>

int main(int argc, char **argv) {
  captureSource* cam1 = setupCameraSource(argv[1]);
  captureSource* cam2 = setupCameraSource(argv[2]);

  openLoopback(argv[3]);

  std::thread cam1Thread(readFrameLoop, cam1);
  std::thread cam2Thread(readFrameLoop, cam2);

  compareFrames(std::vector<captureSource*>{ cam1, cam2 });

  return 0;
}

int debugMain(int argc, char **argv) {
  captureSource *cam1 = setupCameraSource(argv[1]);

  while (true)
  {
    cam1->cap.read(cam1->frame);
    debugFaceDetection(cam1->frame);
    cv::imshow("X", cam1->frame);

    cv::waitKey(10);
  }

  return 0;
}
