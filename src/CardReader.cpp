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
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void delay(int ms){
#ifdef WIN32
  Sleep(ms);
#else
  usleep(ms*1000);
#endif
}

#include "CardReader.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <sstream>
#include <cstdlib> // For getenv

#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "SPI.h"
#include "Serial.h"

#include "SecurityTag.h"

#define RST_PIN  RPI_V2_GPIO_P1_22 // RST pin
#define SS_PIN RPI_V2_GPIO_P1_24 // SDA pin

/**
 * Constructor.
 */
// CardReader::CardReader():MFRC522Extended::MFRC522Extended(){
//   PCD_Init();
// };

/**
 * Constructor.
 * Prepares the output pins.
 */
CardReader::CardReader(RPiGPIOPin SS_PIN, RPiGPIOPin RST_PIN):MFRC522Extended::MFRC522Extended((byte)SS_PIN, (byte)RST_PIN){
  PCD_Init(SS_PIN, RST_PIN);
  // dump info about the connected reader
  printf("Connected\n");
  PCD_DumpVersionToSerial();
  network_server.signal_card_reader.connect(sigc::mem_fun(*this, &CardReader::on_signal_received));
}

/**
 * Destructor.
 */
CardReader::~CardReader(){
  // book keeping
  // PICC_HaltA(); 
  // PCD_StopCrypto1();

}

void CardReader::on_signal_received(const std::string &command, const std::string &data){
   std::cout<<" on_signal_received_fired command "<< command.c_str()<<"\t data "<<data.c_str()<<std::endl;
   // now in network thread processing
  std::string answer;
  while (!answer.length()) 
  {
      while(PICC_IsNewCardPresent()&& answer.empty())
      {
        MFRC522Extended::PICC_Type piccType = PICC_GetType(this->uid.sak);
        // auto *device_name = PICC_GetTypeName(piccType).c_str();
        answer = PICC_GetTypeName(piccType).c_str();
        // memccpy(answer,device_name,sizeof(device_name) , 0);
        
      }

      this->PICC_HaltA();
      this->PCD_StopCrypto1();

  }
  

  // PICC_HaltA();
  // PCD_StopCrypto1();
    network_server.signal_data_received.emit((const std::string&)answer);
 
  // if(this->PICC_IsNewCardPresent() && this->PICC_ReadCardSerial()){
    
  //   auto answer = get_tag_info();
  //   network_server.signal_data_received.emit((const std::string&)answer);
    
  // }else{
      // PICC_HaltA();
      // PCD_StopCrypto1();
  // }


}
/*
* try authenticate against given key and byte block
*/

bool CardReader::try_key(MFRC522Extended::MIFARE_Key *key, byte block, bool dump_hex)
{
    bool result = false;
    byte buffer[18];
    MFRC522Extended::StatusCode status;
    status = PCD_Authenticate(MFRC522Extended::PICC_CMD_MF_AUTH_KEY_A, block, key, &(this->uid));
    if (status != MFRC522Extended::STATUS_OK) {
        return false;
    }

    // Read block
    byte byteCount = sizeof(buffer);
    status = MIFARE_Read(block, buffer, &byteCount);
    if (status != MFRC522Extended::STATUS_OK) {
      return false;
    }
    else {
        // Successful read
        result = true;
        if(dump_hex){
          printf("\n");
          for (byte i = 0; i <  16; i++) {
              printf("%s%02X", i == 0 ? "" : " ", buffer[i]);

          }
        }

    }
    return result;
}

/*
* recursive loop through key from manufacturer's keychain
*/

MFRC522Extended::MIFARE_Key CardReader::generate_key(byte block, bool dump_hex){

  MFRC522::MIFARE_Key default_key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  // Try the known default keys
  for (byte k = 0; k < NR_KNOWN_KEYS; k++) {
      // Copy the known key into the MIFARE_Key structure
      for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
          default_key.keyByte[i] = this->KeyChain[k][i];
      }
      // Try the key
      if (try_key(&default_key, block, dump_hex)) {
          // Found and reported on the key and block,
          // no need to try other keys for this PICC
          return default_key;
      }
  }

  return default_key;

}

/*
  Reads MIFARE type 1kb or 4kb
*/
void CardReader::read_block(byte block, bool dump_hex){
  // MFRC522::MIFARE_Key default_key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  MFRC522Extended::MIFARE_Key default_key =  generate_key(block, dump_hex);

  // byte block;
  byte len;
  MFRC522Extended::StatusCode status;


  byte buffer[18];

  // block = 8;
  len = 18;
  status = PCD_Authenticate(MFRC522Extended::PICC_CMD_MF_AUTH_KEY_A, block, &default_key, &(this->uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522Extended::STATUS_OK) {
    printf("Authentication failed: %s\n", GetStatusCodeName(status).c_str());
    return;
  }
  

  status = MIFARE_Read(block, buffer, &len);
  if (status != MFRC522Extended::STATUS_OK) {
    printf("Read failed: %s\n", GetStatusCodeName(status).c_str());
    return;
  }
  
  printf("\n block => %d ASCII TEXT :", block);
  for (uint8_t i = 0; i < 16; i++)
  {
      printf("%c", buffer[i]); // ascii char
      // printf(" %hhX ", buffer[i]); // ascii char

  }
  
}

/*
  Writes MIFARE type 1kb or 4kb
*/

void CardReader::write_block(byte block){
  std::string userInput;
  MFRC522Extended::StatusCode status;
  byte buffer[16];
  byte bufferLen = sizeof(buffer);
  printf("\nText to Write:\t"); 
  std::getline(std::cin>>std::ws, userInput);
  uint8_t inputLen = userInput.length(); //sizeof(userInput);
  
  for (size_t i = 0; i < inputLen; i++)
  {
      buffer[i] = byte(userInput[i]);
  }
  for (byte i = inputLen; i < bufferLen; i++) buffer[i] = ' ';  // pad spaces
  status = MIFARE_Write(block, buffer, 16);
  if (status != MFRC522Extended::STATUS_OK) {
    printf("Write failed: %s\n", GetStatusCodeName(status).c_str());
    return;
  }
  printf("\nWrite successful\n");

}

/*
  Dumps card type and UID
*/

void CardReader::dump_tag_info(){
      MFRC522Extended::PICC_Type piccType = PICC_GetType(this->uid.sak);
      printf("Card Detected %s:", PICC_GetTypeName(piccType).c_str());
    
      for(byte i = 0; i < this->uid.size; ++i){
        if(this->uid.uidByte[i] < 0x10){
        printf(" 0");
        printf("%X",this->uid.uidByte[i]);
        }
        else{
        printf(" ");
        printf("%X", this->uid.uidByte[i]);
        }
    }
  printf("\n");

}

/*
  return card type and UID
*/

std::string CardReader::get_tag_info(){
      MFRC522Extended::PICC_Type piccType = PICC_GetType(this->uid.sak);
      printf("Serializing info... %s:", PICC_GetTypeName(piccType).c_str());
      
      // std::ostringstream formattedString;
      // for (size_t i = 0; i < uid.size; ++i) {
      //     if (uid.uidByte[i] < 0x10) {
      //         formattedString << " 0" << std::hex << std::setw(2) << static_cast<int>(uid.uidByte[i]);
      //     } else {
      //         formattedString << " " << std::hex << std::setw(2) << static_cast<int>(uid.uidByte[i]);
      //     }
      // }
      char formattedString[2048]; // Adjust the buffer size as needed

      // Initialize the formattedString buffer
      formattedString[0] = '\0';

      for (size_t i = 0; i < uid.size; ++i) {
        sprintf(formattedString + strlen(formattedString), " %02X", static_cast<int>(uid.uidByte[i]));
      }

      // std::string result = formattedString.str();
      std::string result = (std::string)(formattedString);
      std::string response = PICC_GetTypeName(piccType).c_str()+ result;
      return response;

}
