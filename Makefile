# Compiler and flags
CXX = g++
CXXFLAGS = -w -std=c++17
PKGCONFIG = $(shell which pkg-config)
UNAME := $(shell uname)

CFLAGS = 

# Source files and target
SRC_DIR = .
SRC_FILES = $(wildcard $(SRC_DIR)/src/*.cpp) $(wildcard $(SRC_DIR)/src/**/*.cpp) $(SRC_DIR)/main.cpp
# SRC_FILES = MFRC522.cpp main.cpp
TARGET = rfid_admin

# Conditionally set include directories based on the operating system
ifeq ($(UNAME), Linux)
    CFLAGS += -I/usr/include/sigc++-2.0/sigc++
    CFLAGS += $(shell $(PKGCONFIG) sigc++-2.0 --cflags  --libs )
    CXXFLAGS +=-pthread
endif
ifeq ($(UNAME), Darwin)  # macOS
    # CFLAGS += -I/usr/local/include/sigc++-3.0/sigc++
    # CFLAGS += $(shell $(PKGCONFIG) sigc++-3.0 --cflags  --libs )
endif
ifeq ($(findstring CYGWIN, $(UNAME)), CYGWIN)
    # adjust windows headers
    # INCLUDE_DIRS += -I/cygwin/include/directory
endif


# links
BCM2835_LIB = bcm2835
SSL_LIB = ssl 
CRYPTO_LIB = crypto

INCLUDE_DIRS = -Isrc/ -Isrc/RPi-Arduino-SPI/ -Isrc/mfrc522/ #-I/usr/local/include/

default: all

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC_FILES) $(INCLUDE_DIRS) $(CFLAGS) -l$(BCM2835_LIB)

clean:
	rm -f $(TARGET)
