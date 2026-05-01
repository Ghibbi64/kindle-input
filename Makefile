CXX = g++
CXXFLAGS = -lssh -Wall -Wextra -std=c++17 -Isrc -Isrc/dependencies

TARGET = kindle-input

SRCS = $(shell find src -name '*.cpp')
OBJS = $(SRCS:.cpp=.o)
.PHONY: all clean
all: $(TARGET)
	@$(MAKE) -s clean-objs

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean-objs:
	find . -name "*.o" -type f -delete

clean:
	find . -name "*.o" -type f -delete
	rm -f $(TARGET)
