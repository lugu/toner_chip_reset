
#include <Wire.h>

void writeEEPROM(int eeprom, unsigned int address, byte data ) {
	Wire.beginTransmission(eeprom);
	Wire.write((int)(address >> 8));   // MSB
	Wire.write((int)(address & 0xFF)); // LSB
	Wire.write(data);
	Wire.endTransmission();
}

byte readEEPROM(int eeprom, unsigned int address) {
	byte rdata = 0xFF;
	Wire.beginTransmission(eeprom);
	Wire.write((int)(address >> 8));   // MSB
	Wire.write((int)(address & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(eeprom,1);
	if (Wire.available()) {
		rdata = Wire.read();
	}
	return rdata;
}

void setup(void){
	Wire.begin();
	Wire.setClock(400000L);

	Serial.begin(115200);
	while (!Serial.available()) {
		; // wait for serial port to connect
	}
	Serial.println("Let's start!");

	unsigned int eeprom; // 0x50 = 80 = 1010000
	for(eeprom = 0x50; eeprom < 0x50+8; eeprom++) {
		Serial.print("EEPROM ");
		Serial.println(eeprom);
		unsigned int address = 0;
		for(address = 0; address < 64; address++) {
			Serial.print(readEEPROM(eeprom, address), HEX); 
			Serial.print(", ");
		}
		Serial.println("");
	}
}

void loop(){
}
