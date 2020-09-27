SRC_DIR := ./src
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)

build:
	mkdir -p bin
	g++ $(SRC_FILES) -o bin/main -Iinclude -lopencv_core -lopencv_objdetect -lopencv_imgproc -lopencv_videoio -lpthread

run:
	./bin/main "/dev/video0" "/dev/video3" "/dev/video22"
