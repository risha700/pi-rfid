#include <cstddef>
#include <cstdio>
#include "bcm2835.h"
#include "MFRC522.h"


#define RST_PIN  RPI_V2_GPIO_P1_22 // RST pin
#define SS_PIN RPI_V2_GPIO_P1_24 // SDA pin



class CardReader:public MFRC522
{
private:
    /* data */
    
public:
    CardReader(): MFRC522::MFRC522(){
      PCD_Init();
    };
    CardReader(RPiGPIOPin SS_PIN, RPiGPIOPin RST_PIN):MFRC522::MFRC522((byte)SS_PIN, (byte)RST_PIN){
      PCD_Init(SS_PIN, RST_PIN);
      // dump info about the connected reader
      PCD_DumpVersionToSerial();
    };

    ~CardReader(){
      // book keeping
      PICC_HaltA(); 
      PCD_StopCrypto1();
    }
};




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