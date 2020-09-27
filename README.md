# Positional Camera
**Automatic switch for multiple cameras using face detection**

This project uses OpenCV to capture multiple webcams and detect to the one the user is looking at. It streams the selected camera to another device, which can be a [v4l2loopback](https://github.com/umlaeute/v4l2loopback).

It's useful to video conferences with multiple monitors, when you look away and constantly lose eye contact with other participants.

## Dependencies

- [opencv](https://github.com/opencv/opencv)
- [v4l2loopback](https://github.com/umlaeute/v4l2loopback) (optional)

## Building

```
make
```

## Usage

```
Automatic switch for multiple cameras using face detection
Usage:
  positional-camera [OPTION...]

  -i, --input arg   Input source (e.g. /dev/video0). Can be specified
                    multiple times.
  -o, --output arg  Output target (e.g. /dev/video2).
  -w, --width arg   Output's width in pixels (default: 640)
  -h, --height arg  Output's height in pixels (default: 480)
  -f, --fps arg     Output's FPS (default: 30)
  -r, --rounds arg  Amount of comparison rounds a camera must win to be
                    selected as source (default: 3)
      --help        Show help
```

Example, retrieving data from multiple webcams (`/dev/video0` and `/dev/video3`) and outputing to virtual camera (`/dev/video22`):

```
output/positional-camera -i /dev/video0 -i /dev/video3 -o /dev/video22
```
