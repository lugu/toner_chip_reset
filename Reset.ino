
#include <Wire.h>
#define eeprom 0x50

void writeEEPROM(int device, unsigned int address, byte data ) {
  Wire.beginTransmission(device);
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
}

byte readEEPROM(int device, unsigned int address) {
  byte rdata = 0xFF;
  Wire.beginTransmission(device);
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(device,1);
  if (Wire.available()) {
	  rdata = Wire.read();
  }
  return rdata;
}

void setup(void){
  Wire.begin();

  Serial.begin(9600);
  while (!Serial.available()) {
	  ; // wait for serial port to connect
  }
  Serial.println("Let's start!");

  unsigned int address = 0;
  Serial.println("Reading first bytes of the EEPROM");

  for(address = 0; address<10; address++) {
     Serial.print(readEEPROM(eeprom, address), HEX); 
     Serial.print(", ");
  }
}

void loop(){
}
