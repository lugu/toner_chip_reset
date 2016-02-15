
Install
=======

	$ cd reset
	$ make
	$ make upload
	$ picocom -b 115200 /dev/ttyACM0

Done
====

* Create arduino hello world
* Read internal EEPROM
* Draw the cricuit
* Understand the cricuit
* Try i2c clock at 400kHz and 1MHz
* Scan for device
* Analyse I2C trame with a logical analyser

Todo
=====

* Read one EEPROM datasheet
* Verifies the timming between read and write operations
* Find the exact EEPROM chip model
* Find a dummy i2c device to validate the communication
* Communicate with the dummy i2c device
* Find the EEPROM address
* Read the EPPROM chip
* Analyse the EEPROM dump
* Make a data hypothesis
* Disable the write protection
* Write the EEPROM
* Test with the printer



Address
=======

1010 + A2 + A1 + A0

A0 = 1
A1 = 0
A2 = 0

1010001 = 81

A0, A1, A2 can be not used if the eeprom has more than 2K memory.
STOP condition mandatory between writes.
Write cycle: 5 ms.


Random read:

	master send start condition
	master send eeprom address + read bit
	master send data address
	master send start condition
	master send eeprom address + read bit
	device respond with data
	master send stop condition


Connections
===========

Depending on the board the i2c pins are:

	Board           I2C / TWI pins
	Uno, Ethernet   A4 (SDA), A5 (SCL)
	Mega2560        20 (SDA), 21 (SCL)
	Leonardo        2 (SDA), 3 (SCL)
	Due             20 (SDA), 21 (SCL), SDA1, SCL1

Links
=====

Blog:
	http://www.hobbytronics.co.uk/arduino-external-eeprom
	http://www.hobbytronics.co.uk/eeprom-page-write
	http://lusorobotica.com/index.php/topic,461.msg2738.html

Arduino:
	https://www.arduino.cc/en/Reference/Wire

Tonner investigations:
	http://www.mikrocontroller.net/topic/369267
	https://esdblog.org/ricoh-sp-c250dn-laser-printer-toner-hack/

Ricoh:
	https://www.techdata.com/business/Ricoh/files/july2014/CurrentMSRP.pdf
	http://support.ricoh.com/bb_v1oi/pub_e/oi/0001044/0001044844/VM1018655/M1018655.pdf

Datasheets:
	http://www.gaw.ru/pdf/Rohm/memory/br24l01.pdf
	http://www.rinkem.com/web/userfiles/productfile/upload/201009/FM24C02B-04B-08B-16B.pdf

Logical Analyser & I2C:
	http://support.saleae.com/hc/en-us/articles/202740085-Using-Protocol-Analyzers
	http://support.saleae.com/hc/en-us/articles/200730905-Learn-I2C-Inter-Integrated-Circuit


Ricoh SP112 LED
===============

	Left LED: Power Indicator
	This indicator lights up blue when the machine is turned on.
	It flashes when a print job is received and while printing is in progress.

	Right LED: Alert Indicator
	This indicator lights up red when the machine runs out of paper or consumables,
	when the paper settings do match the settings specified by the driver,
	or when other abnormalities occur.

