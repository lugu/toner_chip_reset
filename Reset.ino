
#include <EEPROM.h>

int address = 0;
byte value;

void setup() {
	Serial.begin(9600);
	while (!Serial.available()) {
		; // wait for serial port to connect
	}
	Serial.println("Let's start!");
}

void loop() {

	value = EEPROM.read(address);

	Serial.print("address: ");
	Serial.print(address, HEX);
	Serial.print(" value: ");
	Serial.print(value, HEX);
	Serial.print("\r\n");

	address++;

	delay(200);
}
