CXX = g++
CXXFLAGS = -O1 -std=c++20 -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -I./include
LDFLAGS = -L/opt/homebrew/lib
LIBS = -lFreeImage -lSDL2

TARGET = main
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
