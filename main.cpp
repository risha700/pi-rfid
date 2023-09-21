/**
 * RFID MFRC522 manipulation
 */
#include <cstdio>
#include "CardReader.h"
#include "ConsoleMenu.h"
#include "SecurityTag.h"

#define RST_PIN  RPI_V2_GPIO_P1_22 // RST pin
#define SS_PIN RPI_V2_GPIO_P1_24 // SDA pin



int main() {
  CardReader rfid(SS_PIN, RST_PIN);
  // byte block;
  byte len=18;
  MFRC522::StatusCode status;
  bool card_present;
  byte buffer[18];
  byte blocks[4] = {8,9,10,11};
  
  ConsoleMenu menu(&rfid);
  // int32_t value;

  // while (1) {
  //       // Look for a card
  //   card_present = rfid.PICC_IsNewCardPresent();
  //   if(!card_present)
  //     continue;
  //   card_present =  rfid.PICC_ReadCardSerial();
  //   if(!card_present)
  //     continue;
    
  //   // rfid.dump_tag_info();
    
  //   // if(card_present){
  //   // for(byte blk=0;blk<sizeof(blocks);blk++){
  //   //     rfid.read_block((byte)blocks[blk]);
  //   //     // rfid.PICC_HaltA();
  //   //     // rfid.PCD_StopCrypto1();
  //   // }    
  //     printf("\n");

  //   // }
 
    
      
  //     // rfid.PICC_DumpDetailsToSerial(&rfid.tag);
  //     // rfid.PICC_DumpToSerial(&rfid.tag);
  //     delay(500);
  //     // release bus
      // rfid.PICC_HaltA();
      // rfid.PCD_StopCrypto1();
  // }
  return 0;
}