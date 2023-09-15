// Adapted from
// https://github.com/miguelbalboa/rfid/blob/master/examples/ReadUidMultiReader/ReadUidMultiReader.ino

#include <cstdio>
#include "MFRC522.h"
#include "Arduino.h"
#include "bcm2835.h"

#define RST_PIN  RPI_V2_GPIO_P1_22             // Configurable, see typical pin layout above
#define SS_PIN RPI_V2_GPIO_P1_24 // Configurable, see typical pin layout above

/**
 * Helper routine to dump a byte array as hex values.
 */
void printHex(uint8_t *buffer, size_t bufferSize) {
  for (size_t i = 0; i < bufferSize; i++) {
    printf(" %02X", buffer[i]);
  }
}
void dump_info(MFRC522 NFC){
      MFRC522::PICC_Type piccType = NFC.PICC_GetType(NFC.uid.sak);
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
int main() {
  MFRC522 mfrc522(SS_PIN, RST_PIN);
  mfrc522.PCD_Init();
  // mfrc522.PCD_Init();
  bool status;
  // mfrc522.PCD_Init();
  printf("UID %0X\n", mfrc522.uid);
  mfrc522.PCD_DumpVersionToSerial();
  printf("Place a key tag\n");
  
  while (1) {
        // Look for a card
    status = mfrc522.PICC_IsNewCardPresent();
    if(!status)
      continue;
    
    status = mfrc522.PICC_ReadCardSerial();
     if(!status)
      continue;

    if(status) {
      mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

    }
      // dump_info(mfrc522);
      // mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
      delay(500);

  }

  return 0;
}
