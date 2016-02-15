
#include <Wire.h>

void writeEEPROM(int eeprom, unsigned int address, byte data ) {
	Wire.beginTransmission(eeprom);
	Wire.write((int)(address >> 8));   // MSB
	Wire.write((int)(address & 0xFF)); // LSB
	Wire.write(data);
	Wire.endTransmission();
}

// Set current address:
//	master send start condition
//	master send eeprom address + read bit
//	master send data address
//	master send start condition
unsigned int setCurrentAddress(int eeprom, unsigned int address) {
	Wire.beginTransmission(eeprom);

	byte size = Wire.write(address);
	if (size == 0) {
		Serial.println("Failed to write address");
		return 10;
	}
	byte error = Wire.endTransmission(false);
	if (error == 0) {
		// Serial.println("tranmission success");
	} else if (error == 1) {
		Serial.println("data too long to fit in buffer");
	} else if (error == 2) {
		Serial.println("receive NAK when transmiting address");
	} else if (error == 3) {
		Serial.println("receive NAK when transmiting data");
	} else if (error == 4) {
		Serial.println("other error");
	} else {
		Serial.println("unknown error");
	}

	// return error value
	return error;
}

// Current read:
// 	master send eeprom address + read bit
// 	device respond with data
// 	master send stop condition
unsigned int printCurrentAddress(int eeprom) {
	byte size = Wire.requestFrom(eeprom, 1, true);
	if (size == 0) {
		// Serial.println("no data receive from device");
	}
	if (Wire.available()) {
		byte rdata = Wire.read();
		Serial.print("0x");
		Serial.print(rdata, HEX);
		Serial.println("");
		return 0;
	} else {
		return 1;
	}
}

// Random read:
//	1. set current address
//	2. read current address
unsigned int printRandomAddress(int eeprom, unsigned int address) {

	if (setCurrentAddress(eeprom, address) != 0) {
		Serial.println("failed to set current address");
		return 1;
	}

	if (printCurrentAddress(eeprom) != 0) {
		Serial.println("failed to read current address");
		return 2;
	}
	return 0;
}

int signalPin = 30; // use this pin to time sequences

// swith on/off n times the digital pin
void bip(unsigned int pin) {
	digitalWrite(pin, HIGH);
	delay(1);
	digitalWrite(pin, LOW);
	delay(1);
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

	pinMode(signalPin, OUTPUT);
}

void loop(){
	unsigned int eeprom = 0x50; // 0x50 = 80 = 1010000
	for(eeprom = 0x50; eeprom < 0x58; eeprom++) {
		unsigned int address;
		Serial.print("device ");
		Serial.print(eeprom, HEX);
		Serial.println("");

		bip(signalPin); // signal begining of a sequence
		for(address = 0; address < 128; address++) {
			if (printRandomAddress(eeprom, address) != 0) {
				break;
			}
		}
	}
}
