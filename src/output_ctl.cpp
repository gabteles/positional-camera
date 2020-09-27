#include <camera_ctl.hpp>

OutputWriter::OutputWriter(string output, int width, int height, int fps, IVideoSource *source) {
  this->source = source;
  this->frameRater = new FrameRater(fps);
  this->outputFd = open(output.c_str(), O_WRONLY);
  cout << "[OUTPUT] FD: " << outputFd << endl;

  this->setupOutput(width, height);
}

void OutputWriter::setupOutput(int width, int height) {
  int res;
  int channels = 3;
  int total = width * height;

  v4l2_format format;
  format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  res = ioctl(this->outputFd, VIDIOC_G_FMT, &format);
  cout << "[OUTPUT] Setup ioctl 1: " << res << endl;
  format.fmt.pix.field = V4L2_FIELD_NONE;
  format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
  format.fmt.pix.width = width;
  format.fmt.pix.height = height;
  format.fmt.pix.bytesperline = width * channels;
  format.fmt.pix.sizeimage = total * channels;

  res = ioctl(this->outputFd, VIDIOC_S_FMT, &format);
  cout << "[OUTPUT] Setup ioctl 2: " << res << endl;
}

void OutputWriter::outputFrame() {
  this->source->executeWithFrame([this](Mat frame) {
    // TODO: cvt should be called when frames are read
    cv::Mat colorCorrectFrame;
    cv::cvtColor(frame, colorCorrectFrame, CV_BGR2YUV_I420);
    write(
      this->outputFd,
      colorCorrectFrame.data,
      colorCorrectFrame.total() * colorCorrectFrame.elemSize()
    );
  });
}

void OutputWriter::outputLoop() {
  while (true) {
    this->outputFrame();
    this->frameRater->sleep();
  }
}
