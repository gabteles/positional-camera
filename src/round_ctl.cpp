#include <camera_ctl.hpp>
#include <algorithm>
#include <iterator>

const int majorityRounds = (OP_ROUNDS / 2);

std::string formatResult(std::vector<int> scores) {
  std::stringstream *ss = new std::stringstream();
  *ss << "[" << scores[0];
  std::for_each(std::next(scores.begin()), scores.end(), [ss](int score) {
    *ss << ", " << score;
  });
  *ss << "]";

  return ss->str();
}

void compareFrames(std::vector<captureSource*> cams) {
  std::vector<int> faceScores(cams.size());

  std::vector<int> roundResults(cams.size());
  std::fill(roundResults.begin(), roundResults.end(), 0);

  std::vector<double> fps(cams.size());
  std::transform(cams.begin(), cams.end(), fps.begin(), [](captureSource *cam) { return cam->fps; });
  double targetFps = *std::max_element(fps.begin(), fps.end());
  int frameTimeMs = 1000 / (int)targetFps;

  switchSource(cams[0]);

  using clock = std::chrono::system_clock;
  while (true) {
    std::transform(cams.begin(), cams.end(), faceScores.begin(), [](captureSource *cam) {
      cam->mutex->lock();
      int score = faceDirectionScore(cam->frame);
      cam->mutex->unlock();
      return score;
    });

    auto result = std::minmax_element(faceScores.begin(), faceScores.end());

    // Tie!
    if (*result.first == *result.second) {
      std::this_thread::sleep_for(std::chrono::milliseconds(frameTimeMs));
      continue;
    }

    int maxPosition = result.second - faceScores.begin();
    roundResults[maxPosition]++;

    std::cout << "[ROUND] Round result: " << formatResult(faceScores) << " => " << formatResult(roundResults) << std::endl;

    if (roundResults[maxPosition] > majorityRounds) {
      std::cout << "[ROUND] Cam " << maxPosition << " won battle" << std::endl;
      switchSource(cams[maxPosition]);
      std::fill(roundResults.begin(), roundResults.end(), 0);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(frameTimeMs * 10));
  }
}
