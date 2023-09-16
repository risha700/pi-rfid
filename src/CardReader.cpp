/**
 * 
 * Owner: Ahmed A. <ahbox@outlook.com>
 * Description: CardReader is a wrapper around MFRC522 class supports rasperry-pi GPIO through bcm2835 library
 * 
 * Project: Otago - OPAIC Platform and devices submission 09-2023
 * Source Code: git@github.com:risha700/pi-rfid.git
 * License: MIT 
 * 
 *  
 */


#include "CardReader.h"
#include <cstddef>
#include <cstdio>


#define RST_PIN  RPI_V2_GPIO_P1_22 // RST pin
#define SS_PIN RPI_V2_GPIO_P1_24 // SDA pin

/**
 * Constructor.
 */
CardReader::CardReader():MFRC522::MFRC522(){
  PCD_Init();
};

/**
 * Constructor.
 * Prepares the output pins.
 */
CardReader::CardReader(RPiGPIOPin SS_PIN, RPiGPIOPin RST_PIN):MFRC522::MFRC522((byte)SS_PIN, (byte)RST_PIN){
  PCD_Init(SS_PIN, RST_PIN);
  // dump info about the connected reader
  PCD_DumpVersionToSerial();
}

/**
 * Destructor.
 */
CardReader::~CardReader(){
  // book keeping
  PICC_HaltA(); 
  PCD_StopCrypto1();
}