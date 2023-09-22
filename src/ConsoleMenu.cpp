#include <iostream>
#include <map>
#include <functional>
#include <string>
#include "CardReader.h"
#include "ConsoleMenu.h"

ConsoleMenu::ConsoleMenu(CardReader* rfid)
{
    typedef void (*FnPtr)();
    // std::map<int, FnPtr> menu_options;

    std::map<int, std::string> menu_options;
    menu_options.insert(std::make_pair(1, "rfid.dump_tag_info"));
    menu_options.insert(std::make_pair(2, "rfid.read"));
    menu_options.insert(std::make_pair(3, "rfid.write"));
    menu_options.insert(std::make_pair(4, "rfid.dump_all"));
    menu_options.insert(std::make_pair(5, "rfid.exit"));

    int choice;
    std::string action;

    std::cout << "Place NFC tag! "<<std::endl;
    
    while (true) {
        while(rfid->PICC_IsNewCardPresent() && rfid->PICC_ReadCardSerial())
        {
            // Display the menu options
            std::cout << "\nRFID Card detected \n" << std::endl;
            // Iterate over the key-value pairs in the map
            for (const auto& pair : menu_options) {
                action = pair.second;
                std::cout << pair.first << " -> " << pair.second << std::endl;
            }
            // Prompt the user for their choice
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            // while (menu_options.find(choice) == menu_options.end()) {
            //         std::cout << "Invalid entry, " << choice << "\n try again \t:" << std::endl;
                    
            //     }

            switch (choice) {
                case 1:
                    rfid->dump_tag_info();
                    break;
                case 2:
                    rfid->read_block(8);
                    break;
                case 3:
                    rfid->read_block(8); // 8 or 9 or 10 as per data sheet
                    rfid->write_block(8);
                    break;
                case 4:
                    rfid->PICC_DumpDetailsToSerial(&rfid->tag);
                    rfid->PICC_DumpToSerial(&rfid->tag);
                    break;
                case 5:
                    std::cout << "Exiting Bye Bye..."<< menu_options[choice] << std::endl;
                    exit(0);
                default:
                    std::cout << "Invalid choice. Please try again. \t" << std::endl;
                    std::cin >> choice;
                    break;
            }

        }
        rfid->PICC_HaltA();
        rfid->PCD_StopCrypto1();  
        delay(1000);

    }


}

ConsoleMenu::~ConsoleMenu()
{
}
  
