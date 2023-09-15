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
#include <cstddef>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <sstream>
#include <string>
#include "MFRC522.h"
// using namespace std;

#define NR_KNOWN_KEYS   8
// Known keys, see: https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys
byte knownKeys[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] =  {
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF = factory default
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
    {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
    {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
    {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
    {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
};



void dump_info(MFRC522 NFC){
      byte piccType = NFC.PICC_GetType(NFC.uid.sak);
      printf("Card Detected %s:", NFC.PICC_GetTypeName(piccType).c_str());
    
      for(byte i = 0; i < NFC.uid.size; ++i){
      if(NFC.uid.uidByte[i] < 0x10){
	    printf(" 0");
	    printf("%X",NFC.uid.uidByte[i]);
      }
      else{
	    printf(" ");
	    printf("%X", NFC.uid.uidByte[i]);
      }
    }
  printf("\n");

}
bool try_key(MFRC522 NFC, MFRC522::MIFARE_Key *key, byte block, bool dump_hex=false)
{
    bool result = false;
    byte buffer[18];
    // byte block = 8;
    byte status;

    // Serial.println(F("Authenticating using key A..."));
    status = NFC.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(NFC.uid));
    if (status != MFRC522::STATUS_OK) {
        return false;
    }

    // Read block
    byte byteCount = sizeof(buffer);
    status = NFC.MIFARE_Read(block, buffer, &byteCount);
    if (status != MFRC522::STATUS_OK) {
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

MFRC522::MIFARE_Key generate_key(MFRC522 NFC, byte block=8, bool dump_hex=false){

  MFRC522::MIFARE_Key default_key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  // Try the known default keys
  for (byte k = 0; k < NR_KNOWN_KEYS; k++) {
      // Copy the known key into the MIFARE_Key structure
      for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
          default_key.keyByte[i] = knownKeys[k][i];
      }
      // Try the key
      if (try_key(NFC, &default_key, block, dump_hex)) {
          // Found and reported on the key and block,
          // no need to try other keys for this PICC
          return default_key;
      }
  }

  return default_key;

}

void read_block(MFRC522 NFC, byte block=8, bool dump_hex=false){
  // MFRC522::MIFARE_Key default_key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  MFRC522::MIFARE_Key default_key =  generate_key(NFC, block, dump_hex);

  // byte block;
  byte len;
  byte status;

  byte buffer1[18];

  // block = 8;
  len = 18;
  status = NFC.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &default_key, &(NFC.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    printf("Authentication failed: %s\n", NFC.GetStatusCodeName(status).c_str());
    return;
  }
  

  status = NFC.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    printf("Read failed: %s\n", NFC.GetStatusCodeName(status).c_str());
    return;
  }

  // printf("Read Success: %d\n", status);
  // printf("Buff length %d\n", sizeof(buffer1));
  printf("ASCII TEXT :\t");
  for (uint8_t i = 0; i < 16; i++)
  {
      printf("%c", buffer1[i]); // ascii char

  }
  
}


void write_block(MFRC522 NFC, byte block){
  string userInput;
  byte status;
  byte buffer[16];
  byte bufferLen = sizeof(buffer);
  printf("\nText to Write:\t");
  std::getline(std::cin, userInput);
  uint8_t inputLen = userInput.length(); //sizeof(userInput);
  
  for (size_t i = 0; i < inputLen; i++)
  {
      buffer[i] = byte(userInput[i]);
  }
  for (byte i = inputLen; i < bufferLen; i++) buffer[i] = ' ';  // pad spaces
  status = NFC.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    printf("Write failed: %s\n", NFC.GetStatusCodeName(status).c_str());
    return;
  }
  printf("\nWrite successful\n");

  

  


}

int main(){
  MFRC522 NFC;
  


  NFC.PCD_Init();
  printf("Place a key tag\n");
  while(1){
    // Look for a card
    if(!NFC.PICC_IsNewCardPresent())
      continue;

    if( !NFC.PICC_ReadCardSerial())
      continue;

    dump_info(NFC);

    // default writable blocks on MIFARE 1KB 8,9,10
    // for (size_t i = 0; i <=10; i++)
    // {
    //   read_block(NFC, i, false);
    //   printf("\n");
    // }
    read_block(NFC, 9);

    write_block(NFC, 9);
    
    read_block(NFC, 9);
  
    printf("\n");
    delay(1000);
    NFC.PICC_HaltA();
    NFC.PCD_StopCrypto1();
  }
  
  return 0;
}

