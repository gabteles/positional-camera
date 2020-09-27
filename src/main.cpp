#include <PositionalCamera.hpp>

using namespace PositionalCamera;

typedef struct {
  bool showHelp;
  vector<string> sourceNames;
  string outputTarget;
  int width;
  int height;
  int fps;
  int rounds;
} cliOptions;

cliOptions* parseCliOptions(int argc, char **argv) {
  cxxopts::Options cli("positional-camera", "Automatic switch for multiple cameras using face detection");

  cli.add_options()
    ("i,input", "Input source (e.g. /dev/video0). Can be specified multiple times.", cxxopts::value<vector<string>>())
    ("o,output", "Output target (e.g. /dev/video2).", cxxopts::value<string>())
    ("w,width", "Output's width in pixels", cxxopts::value<int>()->default_value("640"))
    ("h,height", "Output's height in pixels", cxxopts::value<int>()->default_value("480"))
    ("f,fps", "Output's FPS", cxxopts::value<int>()->default_value("30"))
    ("r,rounds", "Amount of comparison rounds a camera must win to be selected as source", cxxopts::value<int>()->default_value("3"))
    ("help", "Show help", cxxopts::value<bool>()->default_value("false"));

  cliOptions *options = new cliOptions;

  try {
    cxxopts::ParseResult result = cli.parse(argc, argv);
    options->showHelp = result["help"].as<bool>();
    options->sourceNames = result["input"].as<vector<string>>();
    options->outputTarget = result["output"].as<string>();
    options->width = result["width"].as<int>();
    options->height = result["height"].as<int>();
    options->fps = result["fps"].as<int>();
    options->rounds = result["rounds"].as<int>();

    if (options->showHelp)
      cout << cli.help();

    return options;
  } catch (std::domain_error err) {
    cerr << cli.help();
  } catch (cxxopts::OptionException err) {
    cerr << err.what() << endl << endl << cli.help();
  }

  return NULL;
}

int main(int argc, char **argv) {
  // Parsing options
  cliOptions *options = parseCliOptions(argc, argv);

  if (!options) return 1;
  if (options->showHelp) return 0;

  // Sources
  vector<IVideoSource *> *sources = new vector<IVideoSource *>;
  for (auto sourceName : options->sourceNames)
    sources->push_back(new WebcamVideoSource(sourceName));

  // Selector
  SourceSelector *selector = new SourceSelector(sources, options->rounds);
  selector->startCapturing();

  // Writer
  OutputWriter *output = new OutputWriter(
    options->outputTarget,
    options->width,
    options->height,
    options->fps,
    selector
  );

  // Run
  output->outputLoop();
  return 0;
}
