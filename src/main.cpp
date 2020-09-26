#include <camera_ctl.hpp>

int main(int argc, char **argv) {
  VideoSource* cam1 = new VideoSource(string(argv[1]));
  VideoSource* cam2 = new VideoSource(string(argv[2]));

  OutputController *output = new OutputController(string(argv[3]), OP_WIDTH, OP_HEIGHT);

  std::thread cam1Thread([cam1]() { cam1->readFrameLoop(); });
  std::thread cam2Thread([cam2]() { cam2->readFrameLoop(); });
  std::thread outputThread([output]() { output->outputLoop(); });

  compareFrames(vector<VideoSource*>{ cam1, cam2 }, output);

  return 0;
}

int debugMain(int argc, char **argv) {
  //captureSource *cam1 = setupCameraSource(argv[1]);
//
  //while (true)
  //{
  //  cam1->cap.read(cam1->frame);
  //  debugFaceDetection(cam1->frame);
  //  cv::imshow("X", cam1->frame);
//
  //  cv::waitKey(10);
  //}
//
  return 0;
}
