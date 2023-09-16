#ifndef CARDREADER_H
#define CARDREADER_H

#include "bcm2835.h"
#include "MFRC522.h"




class CardReader:public MFRC522
{
private:
    /* data */
    
public:
    CardReader();
    CardReader(RPiGPIOPin SS_PIN, RPiGPIOPin RST_PIN);
    ~CardReader();
};

#endif // CARDREADER_H