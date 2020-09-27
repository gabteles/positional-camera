#include <PositionalCamera.hpp>

using namespace PositionalCamera;

vector<IVideoSource*> *getVideoSourcesFromArgs(int argc, char** argv) {
  vector<IVideoSource*> *sources = new vector<IVideoSource*>;

  for (int i = 1; i < (argc - 1); i++)
    sources->push_back(new WebcamVideoSource(string(argv[i])));

  return sources;
}

int main(int argc, char **argv) {
  vector<IVideoSource*> *sources = getVideoSourcesFromArgs(argc, argv);

  SourceSelector *selector = new SourceSelector(sources, 3);
  selector->startCapturing();

  OutputWriter *output = new OutputWriter(
    string(argv[argc - 1]),
    640,
    480,
    30,
    selector
  );

  output->outputLoop();
  return 0;
}
