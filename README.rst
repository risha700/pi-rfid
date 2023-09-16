MFRC522
=======

Raspberry PI library for MFRC522.

Example program, Read.cpp, which will read rfid cards.


This library uses the bcm2835 library found at:
http://www.airspayce.com/mikem/bcm2835/bcm2835-1.50.tar.gz 

Or Alternatively install_bcm script should get it ready:

  sudo install_bcm.sh

To build:

  make all

To Run:

  sudo ./rfid_admin

Pin Layout
----------

The following table shows the pin layout used:
 
+-----------+----------+-------------+
|           | MFRC522  | Raspberry Pi|
+  Signal   +----------+-------------+
|           | Pin      | Pin         |
+===========+==========+=============+
| RST/Reset | RST      | 22          |
+-----------+----------+-------------+
| SPI SS    | SDA      | 24          |
+-----------+----------+-------------+
| SPI MOSI  | MOSI     | 19          |
+-----------+----------+-------------+
| SPI MISO  | MISO     | 21          |
+-----------+----------+-------------+
| SPI SCK   | SCK      | 23          |
+-----------+----------+-------------+
| 3V        | 3v       | 1           |
+-----------+----------+-------------+
| GND       | GND      | 25          |
+-----------+----------+-------------+


**Credits**
  **Thanks** to @luvbum introduced bcm2835 instead of Arduino SPI https://github.com/luvbum/RPi-RFID.git
  
  **Thanks** to @miguelbalboa for the awesome work https://github.com/miguelbalboa/rfid.git

  **Thanks** to @Calvin-LL for the stable BCM2835 -> Arduino API https://github.com/Calvin-LL/RPi-MFRC522.git

  **Thanks** to Apple Inc. for the free BCM2835 library