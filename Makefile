OPENCV_LIBS = $(shell pkg-config --libs opencv)
SRC_DIR := ./src
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)

build:
	mkdir -p bin
	g++ $(SRC_FILES) -o bin/main -Isrc/headers $(OPENCV_LIBS) -lpthread

run:
	./bin/main "/dev/video0" "http://192.168.1.51:4747/mjpegfeed" "/dev/video22"
