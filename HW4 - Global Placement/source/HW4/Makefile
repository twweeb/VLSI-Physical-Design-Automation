CXX := g++
CXXFLAGS := -O3 -Wall -Wextra -Iinclude
LDFLAGS := -Llib -lDetailPlace -lGlobalPlace -lLegalizer -lPlacement -lParser -lPlaceCommon
TARGET := hw4
SRC_DIRS := src
SRCS := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

example: example.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(TARGET) $(OBJS) example
