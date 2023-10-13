#!/bin/bash

curl -kL http://www.airspayce.com/mikem/bcm2835/bcm2835-1.73.tar.gz -O bcm2835.tar.gz

tar -zxvf bcm2835.tar.gz
cd bcm2835

./configure

make

sudo make check

sudo make install

# deps
sudo apt-get install libsystemd-dev -y -qq
sudo apt-get install libspdlog-dev -y -qq