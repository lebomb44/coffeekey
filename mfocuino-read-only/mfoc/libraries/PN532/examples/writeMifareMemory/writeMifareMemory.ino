// This example writes a MIFARE memory block 0x08. It is tested with a new MIFARE 1K cards. Uses default keys.
// Note: Memory block 0 is readonly and contains manufacturer data. Do not write to Sector Trailer block
// unless you know what you are doing. Otherwise, the MIFARE card may be unusable in the future.

//Contributed by Seeed Technology Inc (www.seeedstudio.com)
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#include <PN532.h>

#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
#define SCK 52
#define MOSI 51
#define SS 53
#define MISO 50
#else
#define SCK 13
#define MOSI 11
#define SS 10
#define MISO 12
#endif

PN532 nfc(SCK, MISO, MOSI, SS);

uint8_t written=0;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  Serial.print("Supports "); Serial.println(versiondata & 0xFF, HEX);
  
  // configure board to read RFID tags and cards
  nfc.SAMConfig();
}


void loop(void) {
  uint32_t id;
  // look for MiFare type cards
  id = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A);
  
  if (id != 0) 
  {
    Serial.print("Read card #"); Serial.println(id);
    Serial.println();
    
    uint8_t keys[]= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    uint8_t writeBuffer[16];
    for(uint8_t ii=0;ii<16;ii++)
    { 
       writeBuffer[ii]=ii; //Fill buffer with 0,1,2....F
    }
    if(nfc.authenticateBlock(1, id ,0x08,KEY_A,keys)) //authenticate block 0x08
    {
    //if authentication successful

    if(written == 0) //Not written
    {
    written = nfc.writeMemoryBlock(1,0x08,writeBuffer); // Write writeBuffer[] to block 0x08
    if(written)
      Serial.println("Write Successful");   
    }

    
    uint8_t block[16];
    //read memory block 0x08
    if(nfc.readMemoryBlock(1,0x08,block))
    {
    Serial.println("Read block 0x08:");   
    //if read operation is successful
      for(uint8_t i=0;i<16;i++)
      {
        //print memory block
        Serial.print(block[i],HEX);
        Serial.print(" ");
      }
      Serial.println();
     }
   }
  }
  
  delay(500);
}

