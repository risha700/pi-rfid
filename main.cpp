/**
 * RFID MFRC522 manipulation
 **/
#include <cstdio>
#include "NetworkServer.h"
#include "CardReader.h"
#include "ConsoleMenu.h"
#include "SecurityTag.h"

#define RST_PIN  RPI_V2_GPIO_P1_22 // RST pin
#define SS_PIN RPI_V2_GPIO_P1_24 // SDA pin



int main() {
  CardReader rfid(SS_PIN, RST_PIN);
  NetworkServer network_server;
  network_server.signal_card_reader.connect(sigc::mem_fun(rfid, &CardReader::on_signal_received));
  
  
  ConsoleMenu menu(&rfid);
  
  return 0;
}