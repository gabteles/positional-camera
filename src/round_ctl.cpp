#include <camera_ctl.hpp>

SourceSelector::SourceSelector(
  vector<VideoSource*> *sources,
  OutputController *controller
) {
  this->sources = sources;
  this->controller = controller;
  this->roundResults.resize(sources->size());
  this->sourceScores.resize(sources->size());

  this->resetRound();
  this->initializeFrameRater();
}

void SourceSelector::resetRound() {
  fill(roundResults.begin(), roundResults.end(), 0);
}

void SourceSelector::initializeFrameRater() {
  int targetFps = 0;

  for (auto source : *sources) {
    int sourceFps = source->getFps();

    if (targetFps < sourceFps)
      targetFps = sourceFps;
  }

  // Works at 1/10 times the faster camera.
  this->frameRater = new FrameRater(targetFps / 10);
}

const int majorityRounds = (OP_ROUNDS / 2);

string SourceSelector::formatResult(vector<int> scores) {
  stringstream *ss = new stringstream();
  *ss << "[" << scores[0];
  for_each(next(scores.begin()), scores.end(), [ss](int score) {
    *ss << ", " << score;
  });
  *ss << "]";

  return ss->str();
}

void SourceSelector::selectionLoop() {
  using clock = chrono::system_clock;
  this->controller->switchSource(sources->at(0));

  while (true) {
    this->evaluateSourceScores();
    this->processRound();
    this->frameRater->sleep();
  }
}

void SourceSelector::evaluateSourceScores() {
  transform(sources->begin(), sources->end(), sourceScores.begin(), [](VideoSource *cam) {
    return cam->getFaceDirectionScore();
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
    controller->switchSource(sources->at(winnerCamera));
    fill(roundResults.begin(), roundResults.end(), 0);
  }
}
