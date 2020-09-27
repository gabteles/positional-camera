SRCDIR := src
OBJDIR := output
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := ${subst $(SRCDIR),$(OBJDIR),$(subst .cpp,.o,$(SRCS))}
LDLIBS := -lopencv_core -lopencv_objdetect -lopencv_imgproc -lopencv_videoio -lpthread
TARGET := positional-camera

.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(OBJDIR)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o output/$(TARGET) $(OBJS) $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) -c $< -Iinclude -o $@ $(CXXFLAGS) $(CPPFLAGS)
