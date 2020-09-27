#include <PositionalCamera.hpp>

namespace PositionalCamera {
  SourceSelector::SourceSelector(vector<IVideoSource *> *sources, int majorityRounds) {
    this->sources = sources;
    this->selectedSource = sources->at(0);
    this->majorityRounds = majorityRounds;

    this->roundResults.resize(sources->size());
    this->sourceScores.resize(sources->size());

    this->resetRound();
  }

  Mat SourceSelector::getFrame() {
    return selectedSource->getFrame();
  }

  void SourceSelector::startCapturing() {
    IVideoSource::startCapturing();
    for (auto source : *sources)
      source->startCapturing();
  }

  void SourceSelector::stopCapturing() {
    IVideoSource::stopCapturing();
    for (auto source : *sources)
      source->stopCapturing();
  }

  void SourceSelector::capture() {
    this->evaluateSourceScores();
    this->processRound();
    sleep(1);
    // TODO: Frame rate (?)
    //this->frameRater->sleep();
  }

  void SourceSelector::resetRound() {
    fill(roundResults.begin(), roundResults.end(), 0);
  }

  string SourceSelector::formatResult(vector<int> scores) {
    stringstream *ss = new stringstream();
    *ss << "[" << scores[0];
    for_each(next(scores.begin()), scores.end(), [ss](int score) {
      *ss << ", " << score;
    });
    *ss << "]";

    return ss->str();
  }

  void SourceSelector::evaluateSourceScores() {
    transform(sources->begin(), sources->end(), sourceScores.begin(), [](IVideoSource *cam) {
      int *score = new int;
      cam->executeWithFrame([score](Mat frame) {
        *score = FaceDetector::evaluate(frame);
      });
      return *score;
    });
  }

  int SourceSelector::getRoundWinner() {
    auto result = minmax_element(sourceScores.begin(), sourceScores.end());

    // Tie
    if (*result.first == *result.second)
      return -1;

    return result.second - sourceScores.begin();
  }

  void SourceSelector::processRound() {
    int winnerCamera = this->getRoundWinner();

    if (winnerCamera == -1)
      return;

    roundResults[winnerCamera]++;

    cout << "[ROUND] Round result: "
        << formatResult(sourceScores)
        << " => "
        << formatResult(roundResults)
        << endl;

    if (roundResults[winnerCamera] > majorityRounds) {
      cout << "[ROUND] Cam " << winnerCamera << " won battle" << endl;
      this->selectedSource = sources->at(winnerCamera);
      fill(roundResults.begin(), roundResults.end(), 0);
    }
  }
};
