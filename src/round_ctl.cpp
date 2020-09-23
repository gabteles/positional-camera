#include <camera_ctl.hpp>

const int majorityRounds = (OP_ROUNDS / 2);

void compareFrames(captureSource *cam1, captureSource *cam2) {
  using clock = std::chrono::system_clock;
  while (true) {
    cam1->mutex->lock();
    int faceScore1 = faceDirectionScore(cam1->frame);
    cam1->mutex->unlock();
    cam2->mutex->lock();
    int faceScore2 = faceDirectionScore(cam2->frame);
    cam2->mutex->unlock();

    std::cout << "[ROUND] Battle Result: " << faceScore1 << " (Cam A) x " << faceScore2 << " (Cam B)" << std::endl;
    if (faceScore1 == faceScore2) {
      std::this_thread::sleep_for(std::chrono::milliseconds(OP_COMPARE_MS / 100));
      continue;
    }

    if (faceScore1 > faceScore2) {
      cam1->roundsWon++;
    } else {
      cam2->roundsWon++;
    }

    std::cout << "[ROUND] Partial: CAM A - " << cam1->roundsWon << " x " << cam2->roundsWon << " - CAM B" << std::endl;

    if (cam1->roundsWon > majorityRounds || cam2->roundsWon > majorityRounds) {
      if (cam1->roundsWon > cam2->roundsWon) {
        std::cout << "[ROUND] Cam A won round" << std::endl;
        switchSource(cam1);
      } else {
        std::cout << "[ROUND] Cam B won round" << std::endl;
        switchSource(cam2);
      }

      std::cout << "[ROUND] Starting new round" << std::endl;
      cam1->roundsWon = 0;
      cam2->roundsWon = 0;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(OP_COMPARE_MS));
  }
}
