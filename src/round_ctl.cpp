#include <camera_ctl.hpp>
#include <algorithm>
#include <iterator>

const int majorityRounds = (OP_ROUNDS / 2);

string formatResult(vector<int> scores) {
  stringstream *ss = new stringstream();
  *ss << "[" << scores[0];
  for_each(next(scores.begin()), scores.end(), [ss](int score) {
    *ss << ", " << score;
  });
  *ss << "]";

  return ss->str();
}

void compareFrames(vector<VideoSource*> cams, OutputController *controller) {
  vector<int> faceScores(cams.size());

  vector<int> roundResults(cams.size());
  fill(roundResults.begin(), roundResults.end(), 0);

  vector<double> fps(cams.size());
  transform(cams.begin(), cams.end(), fps.begin(), [](VideoSource *cam) { return cam->getFps(); });
  double targetFps = *max_element(fps.begin(), fps.end());
  int frameTimeMs = 1000 / (int)targetFps;

  controller->switchSource(cams[0]);

  using clock = chrono::system_clock;
  while (true) {
    transform(cams.begin(), cams.end(), faceScores.begin(), [](VideoSource *cam) {
      return cam->getFaceDirectionScore();
    });

    auto result = minmax_element(faceScores.begin(), faceScores.end());

    // Tie!
    if (*result.first == *result.second) {
      this_thread::sleep_for(chrono::milliseconds(frameTimeMs));
      continue;
    }

    int maxPosition = result.second - faceScores.begin();
    roundResults[maxPosition]++;

    cout << "[ROUND] Round result: " << formatResult(faceScores) << " => " << formatResult(roundResults) << endl;

    if (roundResults[maxPosition] > majorityRounds) {
      cout << "[ROUND] Cam " << maxPosition << " won battle" << endl;
      controller->switchSource(cams[maxPosition]);
      fill(roundResults.begin(), roundResults.end(), 0);
    }

    this_thread::sleep_for(chrono::milliseconds(frameTimeMs * 10));
  }
}
