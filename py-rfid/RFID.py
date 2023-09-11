import re
from typing import Any
from time import sleep
import RPi.GPIO as GPIO
# https://www.nxp.com/docs/en/data-sheet/NTAG213_215_216.pdf
from mfrc522 import MFRC522, SimpleMFRC522

COMMON_KEY= [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]

class SimpleReader:
    def __init__(self,*args, **kwargs) -> None:
        self.sreader = SimpleMFRC522()
    def read(self):
        id=None
        try:
            while id is None:
                print("Hold a tag near the reader")
                id, text = self.sreader.read()
                print("ID: %s\nText: %s" % (id,text))
        except KeyboardInterrupt as e:
            print("exception {}".format(e)) 
        finally:
                GPIO.cleanup()
                

    def write(self):
        try:
            text = input('New data:')
            print("Now place your tag to write")
            self.reader.write(text)
            print("Written")
        except KeyboardInterrupt as e:
            print("exception {}".format(e)) 
        finally:
                GPIO.cleanup()
                

class AdvancedReader:
    def __init__(self, *args, **kwargs):
        GPIO.setwarnings(False)
        self.reader = MFRC522()
        self.block_addr = [8, 9, 10]
        
        self.key = COMMON_KEY
        super().__init__(args, kwargs)
          
    def uid_to_num(self, uid):
        n = 0
        for i in range(0, 5):
            n = n * 256 + uid[i]
        return n  
    
    
    def _request_session(self):
        (status, TagType) = self.reader.MFRC522_Request(self.reader.PICC_REQIDL)
        if status == self.reader.MI_OK:
            print("Card detected! - type %s"%TagType)
        return status, TagType    
    
    def _detect_collision(self):
        (status, uid) = self.reader.MFRC522_Anticoll()
        if status == self.reader.MI_OK:   
            print("UID-%s"%uid)
            self.reader.MFRC522_SelectTag(uid)        
        return status, uid
    
    def _dump_info(self, uid):
        self.reader.MFRC522_DumpClassic1K(self.key, uid)
        self.reader.MFRC522_StopCrypto1()
        return 
    
    def read_rfid(self): 
        reader = self.reader
        status =  None
        print("Hold a tag near the reader")
        while status != reader.MI_OK:
            try:
                (status, TagType) = reader.MFRC522_Request(reader.PICC_REQIDL)
                if status == reader.MI_OK:
                    print("Card detected! - type %s"%TagType)
                    # get uid
                    (status, uid) = reader.MFRC522_Anticoll()
                    print("UID-%s"%uid)
                    # This is the default key for authentication
                    if status == reader.MI_OK:
                        id = self.uid_to_num(uid)
                        reader.MFRC522_SelectTag(uid)
                        status = reader.MFRC522_Auth(reader.PICC_AUTHENT1A, 8, self.key, uid)
                        data = []
                        text_read = ""
                        if status == reader.MI_OK:
                            for block_num in self.block_addr:
                                block = reader.MFRC522_Read(block_num) 
                                if block:
                                    data += block
                                    if data:
                                        text_read = ''.join(chr(i) for i in data)
                        else:
                            print('Auth Err')
                               
                    print("TEXT: %s"%text_read, "ID: %s"%id)
                    reader.MFRC522_StopCrypto1()     
            except Exception as e:
                print("exception {}".format(e))
                exit()   
            finally:
                GPIO.cleanup()
                # reader.MFRC522_Reset()     
                
    def dump_info(self):
        status = None
        print("Hold a tag near the reader")
        while status!=self.reader.MI_OK:
            try:
                status, _ = self._request_session()
                if status == self.reader.MI_OK:
                    status, uid = self._detect_collision()
                    print("collision detected uid: {}".format(uid))
                    sleep(1)
                    data = []
                    text_read = ""
                    status = self.reader.MFRC522_Auth(self.reader.PICC_AUTHENT1A, 8, self.key, uid)
                    if status == self.reader.MI_OK:
                        for block_num in self.block_addr:
                            block = self.reader.MFRC522_Read(block_num) 
                            if block:
                                data += block
                                if data:
                                    text_read += ''.join(chr(i) for i in data)
                              
                        print("Text: %s"%text_read)

            except Exception as e:
                print("exception {}".format(e))  
                GPIO.cleanup()
                exit()
            finally:
                # self.reader.MFRC522_Reset()                    
                GPIO.cleanup()
