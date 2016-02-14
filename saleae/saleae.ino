int firstPin = 30;                 // LED connected to digital pin 13
int lastPin = 40;                 // LED connected to digital pin 13

void setup()
{
	Serial.begin(115200);

	unsigned int pin;
	for(pin = firstPin; pin < lastPin; pin++) {
		pinMode(pin, OUTPUT);      // sets the digital pin as output
	}
	Serial.println("Digital ping initalized!");
}

// swith on/off n times the digital pin
void bipN(unsigned int pin, unsigned int times) {
	unsigned int t;
	for(t = 0; t < times; t++) {
		digitalWrite(pin, HIGH);
		delay(1);
		digitalWrite(pin, LOW);
		delay(1);
	}

}

void loop()
{
	unsigned int pin;
	unsigned int wait = 1;
	for(pin = firstPin; pin < lastPin; pin++) {
		Serial.print("Pin ");
		Serial.print(pin);
		Serial.println("");

		bipN(pin, wait);
		wait++;
	}
}

