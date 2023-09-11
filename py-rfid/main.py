import re
from typing import Any
from time import sleep
import RPi.GPIO as GPIO
from mfrc522 import MFRC522, SimpleMFRC522
from RFID import SimpleReader, AdvancedReader



        
class GpioUtils(AdvancedReader, SimpleReader):
    def __init__(self, *args, **kwargs) -> None:
        super().__init__(args, kwargs)
        self.menu_options = {
        1: self.dump_info,
        2: self.read_rfid,
        3: self.read,
        4: self.write,
        5: self.buzz,
        6: exit,
        }
        self.action = None
        self.display_menu()
        
        
    def __exit__(self, exc_type, exc_value, exc_traceback):
        GPIO.cleanup()
        
    def display_menu(self):
        for key in self.menu_options.keys():
            val = re.search(r"(?<=method\s).+?\s", str(self.menu_options[key]))
            print(key, '--',  val.group() if val else self.menu_options[key])
        self.get_action()    
        
        
    def get_action(self):
        choice = int(input("Choose an option:  "))
        while choice not in self.menu_options.keys():
            print("Invalid option  ->", choice)
            choice = int(input("Choose an option:  "))
        self.action = choice
        self.act()
        
    def act(self):
        execute_option = self.menu_options.get(self.action)
        execute_option()
        self.display_menu()
        
    def buzz(self, warn=False):
        GPIO.setmode(GPIO.BCM)
        buzzer=14
        GPIO.setup(buzzer,GPIO.OUT)
        while True:
            GPIO.output(buzzer,GPIO.HIGH)
            print("buzzer playing...")    
            sleep(.5)
            GPIO.output(buzzer,GPIO.LOW)
    
            
if __name__ =="__main__":
    GpioUtils()
