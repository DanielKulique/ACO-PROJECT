CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -I/usr/local/include

TARGET := main
SOURCES := main.cpp Graph.cpp Ant.cpp ACO.cpp Visualizer.cpp

LIBS := -L/usr/local/lib \
        -lraylib \
        -lGL \
        -lm \
        -lpthread \
        -ldl \
        -lrt \
        -lX11 \
        -lXrandr \
        -lXi \
        -lXcursor \
        -lXinerama

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o
