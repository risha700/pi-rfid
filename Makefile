# Compiler and flags
CXX = g++
CXXFLAGS = -w -std=c++11

# Source files and target
SRC_DIR = .
# SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
SRC_FILES = MFRC522.cpp main.cpp
TARGET = rfid_admin

BCM2835_LIB = bcm2835
INCLUDE_DIRS = -I.

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC_FILES) $(INCLUDE_DIRS) -l$(BCM2835_LIB)

clean:
	rm -f $(TARGET)
