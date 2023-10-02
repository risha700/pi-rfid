CXX = g++
CXXFLAGS = -Wall -std=c++17

PKGCONFIG = $(shell which pkg-config)
CFLAGS = $(shell $(PKGCONFIG) gtkmm-4.0 --cflags --libs)
CFLAGS +=$(shell $(PKGCONFIG) spdlog --cflags --libs)
#LIBS = `pkg-config --libs gtkmm-4.0`
SRC_DIR = .
SRC_FILES = $(wildcard $(SRC_DIR)/**.cc)
INCLUDE_DIRS = $(SRC_DIR)
# env must go together DEBUG debug | RELEASE info
APP_ENV =-DAPP_ENV=\"DEBUG\" -DSPDLOG_LEVEL=\"debug\" -DRFID_SERVER_ADDR=\"192.168.1.201\" -DRFID_SERVER_PORT=8080

TARGET = app

default: all

all: $(TARGET)
$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS)  $(APP_ENV) -o $(TARGET) $(CFLAGS) $(SRC_FILES) -I$(SRC_DIR)

clean:
	rm -f app