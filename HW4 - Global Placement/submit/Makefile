CXX := g++
CXXFLAGS := -O3 -g -Wall -Wextra -std=c++11
LDFLAGS := -Llib -lDetailPlace -lGlobalPlace -lLegalizer -lPlacement -lParser -lPlaceCommon
TARGET := hw4
HEADER_DIRS := -I include/
SRC_DIR := src
SRCS := $(wildcard $(SRC_DIR:=/*.cpp))
OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(HEADER_DIRS) -c $< -o $@

example: example.cpp
	$(CXX) $(CXXFLAGS) $(HEADER_DIRS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(TARGET) $(OBJS) example
