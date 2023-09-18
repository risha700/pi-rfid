#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "MFRC522.h"
#include "Serial.h"
#include "SecurityTag.h"



SecurityTag::SecurityTag(MFRC522 *nfcIn)
{
  nfc = nfcIn;
}
void SecurityTag::makePubKeyHex()
{
  uint8_t i;
  for (i = 0; i < pubKeyLen; i++)
    sprintf(&pubKeyHex[i * 2], "%02x", pubKey[i]);
  pubKeyHex[(i * 2) + 1] = 0;
}
void SecurityTag::byteToHex(const uint8_t *data, int numBytes, char* out)
{
  char hex[255];
  int i;
  for (i = 0; i < numBytes; i++)
    sprintf(&hex[i * 2], "%02x", data[i]);
  hex[(i * 2) + 1] = 0;

  memcpy(out, hex, strlen(hex) + 1);
}
void SecurityTag::PrintHexChar(const uint8_t *data, const uint32_t numBytes)
  {
#ifdef ARDUINO
    for (uint8_t i = 0; i < numBytes; i++) {
      if (data[i] < 0x10) {
        Serial.print(" 0");
      } else {
        Serial.print(' ');
      }
      Serial.print(data[i], HEX);
    }
    Serial.print("    ");
    for (uint8_t i = 0; i < numBytes; i++) {
      char c = data[i];
      if (c <= 0x1f || c > 0x7f) {
        Serial.print('.');
      } else {
        Serial.print(c);
      }
    }
    Serial.println("");
#else
    for (uint8_t i = 0; i < numBytes; i++) {
      printf(" %2X", data[i]);
    }
    printf("    ");
    for (uint8_t i = 0; i < numBytes; i++) {
      char c = data[i];
      if (c <= 0x1f || c > 0x7f) {
        printf(".");
      } else {
        printf("%c", c);
      }
    //   printf("\n");
    }
#endif
  }

bool SecurityTag::isNewTagPut() {
  bool stat = false;
  //authentificate via AID
  uint8_t selectApdu[] = {
    0x00, /* CLA */
    0xA4, /* INS */
    0x04, /* P1  */
    0x00, /* P2  */
    0x0D, /* Length of AID  */
    0xD2, 0x76, 0x00, 0x00, 0x04, 0x15, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, /* AID of IFX security controller */
    0x12  /* Le  */
  };
  uint8_t dat[64];
  uint8_t len = sizeof(dat);

  //  if((stat = nfc.PCD_TransceiveData(selectApdu, sizeof(selectApdu), dat, &len))) {
  if ((stat = nfc->PCD_TransceiveData(selectApdu, sizeof(selectApdu), dat, &len, NULL, 0, false))) {
      Serial.print("retlen: "); Serial.println(len);
      PrintHexChar(dat, len);
    }

    return (stat);
  }

  bool SecurityTag::isTagStillPlaced() {
    bool stat = false;
    uint8_t apdu[] = {0};
    uint8_t dat[64];
    uint8_t len = sizeof(dat);

    if ((stat = nfc->PCD_TransceiveData(apdu, sizeof(apdu), dat, &len, NULL, 0, false))) {
      Serial.print("retlen: "); Serial.println(len);
      PrintHexChar(dat, len);
    }

    return (stat);
  }

  bool SecurityTag::getKeyInfo(uint8_t pHandle) {
    bool stat = false;
    uint8_t cmd[] = {
      0x00,   // CLA
      0x16,   // INS GET KEY INFO
      0x00,   // P1  key handle
      0x00,   // P2  RFU
      0x50    // Length of response
    };
    uint8_t dat[80];
    uint8_t len = sizeof(dat);
    memset(&dat, 0, sizeof(dat));
    memset(&pubKey, 0, sizeof(pubKey));

    cmd[2] = pHandle;

    if ((stat = nfc->PCD_TransceiveData(cmd, sizeof(cmd), dat, &len))) {
      Serial.print("responseLength: "); Serial.println(len);
      //nfc.PrintHexChar(dat, len);
      if (len == 2) // error
      {
        Serial.print(" Error Code: "); Serial.println((char)dat[0], (char)dat[1]);
        return 0;
      }
      memcpy(&pubKey[0], &dat[8], pubKeyLen);
    }
    Serial.print("keyinfo: "); Serial.print(stat);
    Serial.print(" , retlen: "); Serial.println(len);

    return (stat);
  }

  bool SecurityTag::generateKey() {
    bool stat = false;
    uint8_t cmd[] = {
      0x00, /* CLA */
      0x02, /* INS GENERATE KEY */
      0x00, /* P1  RFU */
      0x00, /* P2  RFU */
      0x01, /* Length of response  */
    };

    uint8_t dat[64];
    uint8_t len = sizeof(dat);

    if ((stat = nfc->PCD_TransceiveData(cmd, sizeof(cmd), dat, &len))) {
      Serial.print("retlen: "); Serial.println(len);
      //nfc.PrintHexChar(dat, len);
    }

    return (stat);
  }

  bool SecurityTag::signHash(uint8_t pHandle, uint8_t *hash) {
    bool stat = false;
    uint8_t cmd[] = {
      0x00,   // CLA
      0x18,   // INS GET KEY INFO
      0x00,   // P1  key handle
      0x00,   // P2  RFU
      0x20,   // length of data
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // P2  hash data
      0x00    // Length of response
    };

    cmd[2] = pHandle;
    memcpy(&cmd[5], hash, 32);

    uint8_t dat[132];
    uint8_t len = sizeof(dat);
    memset(&dat, 0, sizeof(dat));

    if ((stat = nfc->PCD_TransceiveData(cmd, sizeof(cmd), dat, &len))) {
      int siglen = std::max(len - 10, 0);
      Serial.print("sign, responseLength: "); Serial.println(len);
      //nfc.PrintHexChar(dat, len);
      memcpy(signature, &dat[8], siglen);
      byteToHex(signature, siglen, signatureHex);

    }
    Serial.print("sign, stat= "); Serial.println(stat);
    return (stat);
  }


