#ifndef __POSITIONAL_CAMERA_SOURCE_SELECTOR__
#define __POSITIONAL_CAMERA_SOURCE_SELECTOR__

namespace PositionalCamera {
  class SourceSelector : public IVideoSource {
  public:
    SourceSelector(vector<IVideoSource*> *sources);
    virtual void startCapturing();
    virtual void stopCapturing();

  protected:
    virtual Mat getFrame();
    virtual void capture();

  private:
    void resetRound();
    void initializeFrameRater();
    void evaluateSourceScores();
    int getRoundWinner();
    void processRound();
    string formatResult(vector<int> scores);

    IVideoSource* selectedSource;
    vector<IVideoSource*> *sources;
    vector<int> sourceScores;
    vector<int> roundResults;
    FrameRater *frameRater;
  };
};

#endif
