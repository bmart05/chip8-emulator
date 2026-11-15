CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -MMD -MP $(shell pkg-config --cflags sdl3)
LDLIBS   := $(shell pkg-config --libs sdl3)

TARGET   := sdltest
SRCS     := $(wildcard *.cpp)
OBJS     := $(SRCS:.cpp=.o)
DEPS     := $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

.PHONY: clean run
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)