# Positional Camera

This project uses OpenCV to capture multiple webcams and detect to the one the user is looking at. It streams the selected camera to another device, which can be a [v4l2loopback](https://github.com/umlaeute/v4l2loopback).

It's useful to video conferences with multiple monitors, when you look away and constantly lose eye contact with other participants.

## Dependencies

- [opencv](https://github.com/opencv/opencv)
- [v4l2loopback](https://github.com/umlaeute/v4l2loopback) (optional)

## Building

```
make
```

## Running

```
output/positional-camera source1 source2 ... sourceN output
```

Example, retrieving data from multiple webcams (`/dev/video0` and `/dev/video3`) and outputing to virtual camera (`/dev/video22`):

```
output/positional-camera /dev/video0 /dev/video3 /dev/video22
```
