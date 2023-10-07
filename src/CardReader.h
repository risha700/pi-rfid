/**
 * 
 * Owner: Ahmed A. <ahbox@outlook.com>
 * Description: CardReader is a wrapper around MFRC522 class supports rasperry-pi GPIO through bcm2835 library
 * 
 * Project: Otago - OPAIC Platform and devices submission 09-2023
 * Source Code: git@github.com:risha700/pi-rfid.git
 * License: MIT 
 * 
 *  
 */
#ifndef CARDREADER_H
#define CARDREADER_H
#include "NetworkServer.h" //must be first as it collides with bcm math implementation.
#include "bcm2835.h"
#include "MFRC522Extended.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <map>


#define NR_KNOWN_KEYS   8




class CardReader:public MFRC522Extended
{
private:
    byte KeyChain[NR_KNOWN_KEYS][MFRC522Extended::MF_KEY_SIZE] =  {
        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF = factory default
        {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
        {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
        {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
        {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
        {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
        {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
    };
    
public:
    // CardReader();
    CardReader(RPiGPIOPin SS_PIN, RPiGPIOPin RST_PIN);
    ~CardReader();
    MFRC522Extended::MIFARE_Key generate_key(byte block=8, bool dump_hex=false);
    bool try_key(MFRC522Extended::MIFARE_Key *key, byte block, bool dump_hex=false);
    void read_block(byte block=8, bool dump_hex=false);
    void write_block(byte block);
    void dump_tag_info();
    std::string read_block_serialized(byte block);
    std::string write_block_serialized(byte block, const std::string &data);
    void on_signal_received(const std::string &command, const std::string &data);
    // todo interface
    NetworkServer network_server;
    // serialized
    std::string get_tag_info();

    std::string get_dump_info();
    std::map<std::string, std::function<void()>> signal_handler;
    std::string handle_signal(const std::string& key, const std::string &data);
   

};

#endif // CARDREADER_H