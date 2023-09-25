CXX = g++
CXXFLAGS = -Wall -std=c++17

PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) gtkmm-4.0 --cflags --libs)
#LIBS = `pkg-config --libs gtkmm-4.0`
SRC_DIR = .
SRC_FILES = $(wildcard $(SRC_DIR)/**.cc)
INCLUDE_DIRS = $(SRC_DIR)

TARGET = app

default: all

all: $(TARGET)
$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(CFLAGS) $(SRC_FILES) -I$(SRC_DIR)

clean:
	rm -f app