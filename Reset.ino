
#include <Wire.h>

void writeEEPROM(int eeprom, unsigned int reg, byte data ) {
	Wire.beginTransmission(eeprom);
	Wire.write((int)(reg >> 8));   // MSB
	Wire.write((int)(reg & 0xFF)); // LSB
	Wire.write(data);
	Wire.endTransmission();
}

// returns 0 if a device respond at this address.
int testEEPROM(int eeprom) {
	Wire.beginTransmission(eeprom);

	byte error = Wire.endTransmission();

	if (error == 0) {
		Serial.println("");
		Serial.print("Device found at address 0x");
		Serial.println(eeprom, HEX);
	} else {
		Serial.println("");
		Serial.print("No device at address 0x");
		Serial.println(eeprom, HEX);
		return 1;
	}

	return 0;
}

byte printRegister(int eeprom, unsigned int reg) {
	Wire.beginTransmission(eeprom);

	byte size;
	size = Wire.write((int)(reg >> 8));   // MSB
	if (size == 0)
	{
		Serial.print("Failed to write MSB at address 0x");
		Serial.println(eeprom, HEX);
	}
	size = Wire.write((int)(reg & 0xFF)); // LSB
	if (size == 0)
	{
		Serial.print("Failed to write LSB at address 0x");
		Serial.println(eeprom, HEX);
	}

	byte error = Wire.endTransmission();
	if (error != 0)
	{
		Serial.print("I2C device not found at address 0x");
		Serial.println(eeprom, HEX);
	}

	size = Wire.requestFrom(eeprom,1);
	if (error != 0)
	{
		Serial.print("I2C device did not respond to request at address 0x");
		Serial.println(eeprom, HEX);
	}
	if (Wire.available()) {
		byte rdata = 0xFF;
		rdata = Wire.read();
		Serial.print("0x");
		Serial.print(reg, HEX);
		Serial.print(": ");
		Serial.print(rdata, HEX); 
		Serial.println("");
	} else {
		Serial.print("Failed to read register 0x");
		Serial.print(reg, HEX);
		Serial.println("");
		return 1;
	}
	return 0;
}

void setup(void){

	Serial.begin(115200);
	while (!Serial.available()) {
		; // wait for serial port to connect
	}
	Serial.println("Let's start!");
	Wire.begin();
	Wire.setClock(1000000L);
	Serial.println("I2C bus initalized!");
}

void loop(){
	unsigned int eeprom; // 0x50 = 80 = 1010000
	for(eeprom = 0x0; eeprom < 128; eeprom++) {
		if (testEEPROM(eeprom) != 0) {
			continue;
		}
		unsigned int reg = 0;
		for(reg = 0; reg < 64; reg++) {
			if (printRegister(eeprom, reg) != 0) {
				break; 
			}
		}
	}
}
