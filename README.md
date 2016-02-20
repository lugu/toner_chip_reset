
Install
=======

	make
	make upload
	picocom -b 115200 /dev/ttyACM0

Done
====

* Create arduino hello world
* Read internal EEPROM
* Draw the cricuit
* Understand the cricuit
* Try i2c clock at 400kHz and 1MHz
* Scan for device => use MultiSpeedScanner
* Analyse I2C trame with a logical analyser
* Visualize I2C packets with pulseview
* Read one EEPROM datasheet
* Debug i2c addresses sent (1010001 and not 0101000)
* Verifies the timming between read and write operations
* Find the exact EEPROM chip model
* Find the EEPROM address (0x53)
* Read the EPPROM chip
* Order sp112 reset chip from internet

Todo
=====

* Analyse the EEPROM dump
* Make a data hypothesis
* Dump a new reset chip
* Disable the write protection
* Write the EEPROM with a dump of a new reset chip
* Test with the printer



Address
=======

1010 + A2 + A1 + A0

A0 = 1
A1 = 1
A2 = 0

1010011 = 83

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

device 50
receive NAK failed to set current address

device 51
no data available from device
failed to read current address


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
	http://rumburg.org/printerhack/

Toner chip reset for sale:
	http://www.aliexpress.com/item/chip-FOR-RICOH-imagio-SP-112-SF-chip-MAILING-MACHINE-printer-POSTAGE-printer-for-Ricoh-100/32261857176.html
	http://www.ebay.com/itm/Toner-cartridge-refill-kit-for-Ricoh-Aficio-SP112-SP112SU-SP112SF-407166-non-OEM-/161312940764

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


Dump result
===========

	$ head -n 1024 dump.txt > data; echo "plot 'data'; pause mouse" | gnuplot
	$ cat dump.txt | xargs printf "%02x\n" | xxd -c 1 -p -r > dump.bin
	$ xxd dump.bin | head -n 16
	00000000: 2000 0103 0101 0300 0000 ffff ffff ffff   ...............
	00000010: 1504 4d47 2700 1882 0000 0000 2000 0101  ..MG'....... ...
	00000020: 5830 3235 4d34 3331 3536 3620 0045 0000  X025M431566 .E..
	00000030: 0000 0000 0000 0000 0000 0106 0000 0000  ................
	00000040: 0000 0107 0000 0000 0000 0000 0000 0000  ................
	00000050: 0000 0000 0000 0106 2000 0101 2000 0101  ........ ... ...
	00000060: 0000 0000 0000 0000 0106 0000 0000 0000  ................
	00000070: 000e 715d 1000 1427 0000 0000 0000 0000  ..q]...'........
	00000080: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000090: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000a0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000b0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000c0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000d0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000e0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000f0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
