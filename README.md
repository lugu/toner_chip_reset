
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
* Debug i2c addresses sent (1010001 and not 0101000) ~ frequence to high
* Verifies the timming between read and write operations (5ms)
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


Connections
===========

Depending on the board the i2c pins are:

	Board           I2C / TWI pins
	Uno, Ethernet   A4 (SDA), A5 (SCL)
	Mega2560        20 (SDA), 21 (SCL)
	Leonardo        2 (SDA), 3 (SCL)
	Due             20 (SDA), 21 (SCL), SDA1, SCL1


Address
=======

1010 + A2 + A1 + A0

A0 = 1
A1 = 1
A2 = 0

1010011 = 83

A0, A1, A2 can be not used if the eeprom has more than 2K memory.


Read operation
==============

Sequence for random read:

	master send start condition
	master send eeprom address + read bit
	master send data address
	master send start condition
	master send eeprom address + read bit
	device respond with data
	master send stop condition

STOP condition mandatory between writes.
Write cycle: 5 ms.


Bus frequency
=============

The datasheet of the component FM24C02B indicate an operating clock of 1MHz
at 3.3V.

At 1MHz, no address respond correctly, as well the logical analyser show
strange data passing. Especially it decoded wrong addresses.

	device 50
	receive NAK failed to set current address

	device 51
	no data available from device
	failed to read current address

This discussion highligth the max frequence an Arduino Mega can reach.
1MHz seems to be the upper bound.

http://electronics.stackexchange.com/questions/29457/how-to-make-arduino-do-high-speed-i2c

Setting the clocks to 800kHz works just fine.

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

Data hypothesis
===============

From what the dirver send, the print has access to:
	* the number of page
	* the number of 'dot' per page

The number of 'dot' per page can be sent to zero. On my computer i updated
the driver to send the proper value. This is probably why my tonner run out
ink so quickly: during my experiments i probably sent the wrong values.

The eeprom is a simple data storage. The printer my wants to do:
	* store their the number of printed pages and/or printed dot
	* mark is as used by a particular printer to prevent second hand market
	* mark the date of the first and last usage to make it out of date.
	* the tonner may have the information of the maximum 'dot' capacity (or
	  a count down value).

Here is what could be the printer logical sequence:
	* Power on printer
	* Check if a tonner is present on the i2c bus:
		* Check a device respond
		* Verifies the magic number
		* Check if the tonner is compatible this printer model (sp112)
		* Check if associated with this particular printer
			* if not associated, write the printer PN into the tonner
	* Check if the tonner is usable:
		* Check if the tonner is broken
			* Read a broken information (ex: like an error code on eeprom)?
		* Check if the tonner is too old:
			* Read the first date of usage
			* Read the last date of usage
		* Check if the tonner capacity is exhausted:
			* Read the page count
			* Read the max page information
			* Read the dot count
			* Read the max dot information
	* Set to status OK
	* When printing a new page:
		* Receive the date
		* Receive all the pages in a black/white dot format
		* For each page:
			* Verify if the page counter is exhausted
				* if so mark the tonner a broken (reach max pages)
			* Verify the dot counter is exhausted
				* if so mark the tonner a broken (reach max dots)
			* Compare the date with the first usage
				* if no first usage, mark this date as the first usage
				* if first usage more than duration mark the tonner as broken (too old)
			* Compare the date with the last usage
				* if last usage less than continous mark the tonner as broken (too dry)
			* Increment the dot counter
			* Increment the last usage counter
			* Update the last usage time
			* Print the page

From this we can imagine to see:
	* the dot capacity
	* the dot usage
	* the page capacity
	* the page usage
	* the time of first use
	* the time of last use
	* one or more status register (broken information)

About the dates:
	* year/mount/day is enough
	* if the tonner needs to warm-up, hours/minutes/seconds is also required.

Unix epoch format seems well suited for this need (no calendar juggling).
Here is an example of a date in hexadecimal values:

	$ date --date='@1456056478'
	Sun Feb 21 13:07:58 CET 2016
	$ echo "obase=16; 1456056478" | bc
	56C9A89E

32 bits is enough to live until 2038, which certainly exceeded the expected
life of such product.

Part number: 407431 (0x63787)
http://www.cnet.com/products/ricoh-sp-112-printer-monochrome-laser/specs/

Similar part number can be seen in the following dump (407166):
http://www.mikrocontroller.net/topic/369267



	00000000: 2000 0103 0101 0300 0000 ffff ffff ffff   ...............
			  ^^^^ ^^^^ ^^^^ ^^^^
			  header

	00000010: 1504 4d47 2700 1882 0000 0000 2000 0101  ..MG'....... ...
			  ^^^^           ^^^^
			  increase between two prints

	00000020: 5830 3235 4d34 3331 3536 3620 0045 0000  X025M431566 .E..

	00000030: 0000 0000 0000 0000 0000 0106 0000 0000  ................
			                           ^^^^
			                           increase between two prints

	00000040: 0000 0107 0000 0000 0000 0000 0000 0000  ................
			       ^^^^
			       increase between two prints

	00000050: 0000 0000 0000 0106 2000 0101 2000 0101  ........ ... ...
			                 ^^^^
			                 increase between two prints

	00000060: 0000 0000 0000 0000 0106 0000 0000 0000  ................
			                      ^^^^
			                      increase between two prints

	00000070: 000e 715d 1000 1427 0000 0000 0000 0000  ..q]...'........
			    ^^ ^^^^ ^^^^ ^^^^
			    increase between two prints

	00000080: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000090: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000a0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000b0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000c0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000d0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000e0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000f0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
