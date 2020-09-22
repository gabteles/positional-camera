#include <camera_ctl.hpp>

captureSource *outputSource;
std::mutex outputMutex;
int outputFd;

void openLoopback(char *output) {
  int res;

  outputFd = open(output, O_WRONLY);
  std::cout << "[OUTPUT] FD: " << outputFd << std::endl;

  int width = OP_WIDTH;
  int height = OP_HEIGHT;
  int channels = 3;
  int total = width * height;

  v4l2_format format;
  format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  res = ioctl(outputFd, VIDIOC_G_FMT, &format);
  std::cout << "[OUTPUT] Setup ioctl 1: " << res << std::endl;
  format.fmt.pix.field = V4L2_FIELD_NONE;
  format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
  format.fmt.pix.width = width;
  format.fmt.pix.height = height;
  format.fmt.pix.bytesperline = width * channels;
  format.fmt.pix.sizeimage = total * channels;

  res = ioctl(outputFd, VIDIOC_S_FMT, &format);
  std::cout << "[OUTPUT] Setup ioctl 2: " << res << std::endl;
}

void outputFrame(captureSource* cam) {
  outputMutex.lock();
  if (outputSource == cam) {
    cv::Mat colorCorrectFrame;
    cv::cvtColor(cam->frame, colorCorrectFrame, CV_BGR2YUV_I420);
    write(
      outputFd,
      colorCorrectFrame.data,
      colorCorrectFrame.total() * colorCorrectFrame.elemSize()
    );
  }
  outputMutex.unlock();
}

void switchSource(captureSource* source) {
  outputMutex.lock();
  outputSource = source;
  outputMutex.unlock();
}
