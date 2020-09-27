#include <camera_ctl.hpp>

vector<VideoSource*> *getVideoSourcesFromArgs(int argc, char** argv) {
  vector<VideoSource*> *sources = new vector<VideoSource*>;

  for (int i = 1; i < (argc - 1); i++)
    sources->push_back(new VideoSource(string(argv[i])));

  return sources;
}

int main(int argc, char **argv) {
  vector<VideoSource *> *sources = getVideoSourcesFromArgs(argc, argv);
  vector<thread*> sourceThreads;

  OutputController *output = new OutputController(
    string(argv[argc - 1]),
    OP_WIDTH,
    OP_HEIGHT
  );

  for (int i = 0; i < sources->size(); i++) {
    VideoSource *source = sources->at(i);
    sourceThreads.push_back(new thread([source]() { source->readFrameLoop(); }));
  }

  std::thread outputThread([output]() { output->outputLoop(); });

  SourceSelector selector(sources, output);
  selector.selectionLoop();

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
