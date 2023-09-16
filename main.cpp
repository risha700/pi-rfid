/**
 * Entry point
 */
#include <cstdio>
#include "CardReader.h"

#define RST_PIN  RPI_V2_GPIO_P1_22 // RST pin
#define SS_PIN RPI_V2_GPIO_P1_24 // SDA pin


int main() {
  

  CardReader rfid(SS_PIN, RST_PIN);
  
  while (1) {
        // Look for a card
    
    if(!rfid.PICC_IsNewCardPresent())
      continue;
    
    
     if(!rfid.PICC_ReadCardSerial())
      continue;


      rfid.PICC_DumpToSerial(&(rfid.uid));

      // dump_info(mfrc522);
      // mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
      delay(1000);

  }

  return 0;
}