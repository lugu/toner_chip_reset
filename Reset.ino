
#include <Wire.h>

// dump.h define the content to flash to the eeprom
#include "dump_bin.h"

int signalPin = 30; // use this pin to time sequences

// swith on/off n times the digital pin
void bip(unsigned int pin) {
	digitalWrite(pin, HIGH);
	delayMicroseconds(2);
	digitalWrite(pin, LOW);
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
	}
	if (Wire.available()) {
		byte rdata = Wire.read();
		Serial.print("0x");
		Serial.print(rdata, HEX);
		Serial.println("");
		return 0;
	} else {
		Serial.println("no data available from device");
		return 1;
	}
}

// Current write:
// 	master send eeprom address + write bit
// 	master send data
// 	master send stop condition
unsigned int randomWrite(int eeprom, unsigned int address, byte data) {
	Wire.beginTransmission(eeprom);

	byte size = Wire.write(address);
	if (size == 0) {
		Serial.println("Failed to write address");
		return 1;
	}
	size = Wire.write(data);
	if (size == 0) {
		Serial.println("Failed to write data");
		return 2;
	}

	byte error = Wire.endTransmission(true);
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

	delay(5); // wait 5 ms, a write cycle

	return error;
}

// Random read:
//	1. set current address
//	2. read current address
unsigned int printRandomAddress(int eeprom, unsigned int address) {

	if (setCurrentAddress(eeprom, address) != 0) {
		Serial.println("failed to set current address");
		return 1;
	}
	delay(5); // wait 5 ms between write and read

	if (printCurrentAddress(eeprom) != 0) {
		Serial.println("failed to read current address");
		return 2;
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
	unsigned int clock = 800000L; // 800kHz, or 400kHz or 1MHz;
	Wire.setClock(clock);
	Serial.println("I2C bus initalized!");

	pinMode(signalPin, OUTPUT);
}

void loop(){
	unsigned int eeprom; // 0x53 = 83 = 1010011
	for(eeprom = 0x53; eeprom < 0x54; eeprom++) {
		unsigned int address;
		Serial.print("reading device ");
		Serial.print(eeprom, HEX);
		Serial.println("copy the following into: dump.h");
		Serial.println("byte dump[] = {");

		bip(signalPin); // signal begining of a sequence
		for(address = 0; address < 256; address++) {
			if (printRandomAddress(eeprom, address) != 0) {
				break;
			}
		}
		Serial.println("};");
		bip(signalPin); // signal begining of a sequence
		bip(signalPin); // signal begining of a sequence
		Serial.print("writing device ");
		Serial.print(eeprom, HEX);
		Serial.println("");
		for(address = 0; address < dump_bin_len; address++) {
			if (randomWrite(eeprom, address, dump_bin[address]) != 0) {
				Serial.println("Write failed!");
			}
		}
	}
}
