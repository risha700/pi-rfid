#include "bcm2835.h"
#include "MFRC522.h"




class CardReader:public MFRC522
{
private:
    /* data */
    
public:
    CardReader(): MFRC522::MFRC522(){};
    CardReader(RPiGPIOPin SS_PIN, RPiGPIOPin RST_PIN):MFRC522::MFRC522((byte)SS_PIN, (byte)RST_PIN){};
    ~CardReader();
};