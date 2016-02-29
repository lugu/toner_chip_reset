
#include <Wire.h>

// the content to flash on the eeprom to reset the chip.
// defines:
// 	* unsigned int dump_bin_len;
// 	* unsigned char dump_bin[];
#include "reset_bin.h"

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

// display the content of the eeprom
void eepromRead(unsigned int eeprom) {

	Serial.print("reading device ");
	Serial.print(eeprom, HEX);
	Serial.println("");

	unsigned int address;
	for (address = 0; address < 256; address++) {
		if (printRandomAddress(eeprom, address) != 0) {
			Serial.print("Read failed at ");
			Serial.print(address, HEX);
			Serial.println("!");
			break;
		}
	}
	Serial.println("read end.");
}

// erase the content of the eeprom
void eepromWrite(unsigned int eeprom, unsigned char data[],
		unsigned int data_length) {

	Serial.print("writing device ");
	Serial.print(eeprom, HEX);
	Serial.println("");

	unsigned int address;
	for (address = 0; address < data_length; address++) {
		if (randomWrite(eeprom, address, data[address]) != 0) {
			Serial.print("Write failed at ");
			Serial.print(address, HEX);
			Serial.println("!");
			break;
		}
	}
	Serial.println("write finished.");
}

// initialize serial connection and wait for user input.
// initialize i2c bus after user input.
void initialize(void) {

	Serial.begin(115200);
	while (!Serial.available()) {
		; // wait for serial port to connect
	}
	Serial.println("Let's start!");

	// define the i2c bus speed
	// 400kHz, 800kHz or 1MHz
	unsigned int clock = 800000L;

	Wire.begin();
	Wire.setClock(clock);
	Serial.println("I2C bus initalized!");
}

// erase the content of the eeprom with the value of
void work(void) {

	// eeprom address on the i2c bus
	// 0x53 = 83 = 1010011 = 1010 A0=0 A1=1 A2=1
	unsigned int eeprom = 0x53;

	eepromRead(eeprom);
	eepromWrite(eeprom, dump_bin, dump_bin_len);
	eepromRead(eeprom);
}

void setup(void) {

	initialize();
	work();
}

void loop() {}
