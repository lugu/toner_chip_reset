/*
 * (c) Anil Kumar Pugalia, 2010. Email: email@sarika-pugs.com
 *
 * ATmega48/88/168, ATmega16/32
 * 
 * Example Blink. Toggles all IO pins at 1Hz
 */

#include <avr/io.h>
#include <util/delay.h>

#include <Arduino.h>
#include <Wire.h>

void init_io(void)
{
	// 1 = output, 0 = input
	DDRB = 0b11111111; // All outputs
	DDRC = 0b11111111; // All outputs
	DDRD = 0b11111110; // PORTD (RX on PD0). Just for demo
}

void init_wire(void)
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

int main(void)
{
	init_io();
	init_wire();

	while (1)
	{
		PORTC = 0xFF;
		PORTB = 0xFF;
		PORTD = 0xFF;
		_delay_ms(500);

		PORTC = 0x00;
		PORTB = 0x00;
		PORTD = 0x00;
		_delay_ms(500);
	}

	return 0;
}
