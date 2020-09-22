#include <camera_ctl.hpp>

int main(int argc, char **argv) {
  captureSource* cam1 = setupCameraSource(argv[1]);
  captureSource* cam2 = setupCameraSource(argv[2]);

  openLoopback(argv[3]);

  std::thread cam1Thread(readFrameLoop, cam1);
  std::thread cam2Thread(readFrameLoop, cam2);

  compareFrames(cam1, cam2);

  return 0;
}
