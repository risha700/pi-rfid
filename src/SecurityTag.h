#ifndef SECURITYTAG_H
#define SECURITYTAG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "MFRC522.h"
#include "Serial.h"

class SecurityTag {
  public:
    SecurityTag(MFRC522 *nfcIn);
    MFRC522* nfc;
    uint8_t response[64];
    uint8_t responseLength = sizeof(response);
    enum { NONE, MIFARE, SECURITY };
    uint8_t pubKey[80];  // public key
    char    pubKeyHex[200];
    uint8_t signature[200];
    char    signatureHex[200];
    const uint8_t pubKeyLen = 65;
    bool isNewTagPut();
    bool isTagStillPlaced();
    bool getKeyInfo(uint8_t pHandle);
    bool generateKey();
    bool signHash(uint8_t pHandle, uint8_t *hash);
    void makePubKeyHex();
    void byteToHex(const uint8_t *data, int numBytes, char* out);
    void PrintHexChar(const uint8_t *data, const uint32_t numBytes);
};


#endif SECURITYTAG_H