#include <camera_ctl.hpp>

vector<VideoSource*> *getVideoSourcesFromArgs(int argc, char** argv) {
  vector<VideoSource*> *sources = new vector<VideoSource*>;

  for (int i = 1; i < (argc - 1); i++)
    sources->push_back(new VideoSource(string(argv[i])));

  return sources;
}

int main(int argc, char **argv) {
  vector<VideoSource *> *sources = getVideoSourcesFromArgs(argc, argv);

  SourceSelector *selector = new SourceSelector(sources);
  selector->startCapturing();

  OutputWriter *output = new OutputWriter(
    string(argv[argc - 1]),
    OP_WIDTH,
    OP_HEIGHT,
    OP_FPS,
    selector
  );

  output->outputLoop();
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
