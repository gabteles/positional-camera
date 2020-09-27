TARGET := positional-camera
SRCDIR := src
OBJDIR := output
LDLIBS := -lopencv_core -lopencv_objdetect -lopencv_imgproc -lopencv_videoio -lpthread

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := ${subst $(SRCDIR),$(OBJDIR),$(subst .cpp,.o,$(SRCS))}

.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(OBJDIR)

$(TARGET): include/cxxopts.hpp $(OBJS)
	$(CXX) $(LDFLAGS) -o output/$(TARGET) $(OBJS) $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) -c $< -Iinclude -o $@ $(CXXFLAGS) $(CPPFLAGS)

include/cxxopts.hpp:
	wget https://raw.githubusercontent.com/jarro2783/cxxopts/v2_2/include/cxxopts.hpp -O include/cxxopts.hpp
