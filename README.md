


Usage
=====

	$ make
	$ make upload
	$ picocom -b 115200 /dev/ttyACM0

Todo
====

[*] Create arduino hello world
[*] Read internal EEPROM
[*] Draw the cricuit
[*] Understand the cricuit
[*] Try i2c clock at 400kHz and 1MHz
[*] Scan for device => use MultiSpeedScanner
[*] Analyse I2C trame with a logical analyser
[*] Visualize I2C packets with pulseview
[*] Read one EEPROM datasheet
[*] Debug i2c addresses sent (1010001 and not 0101000) ~ frequence to high
[*] Verifies the timming between read and write operations (5ms)
[*] Find the exact EEPROM chip model
[*] Find the EEPROM address (0x53)
[*] Read the EPPROM chip
[*] Order sp112 reset chip from internet
[*] Analyse the EEPROM dump
[*] Make a data hypothesis
[*] Verify the write function
[*] Dump a new reset chip
[*] Write the EEPROM with a dump of a new reset chip
[*] Test with the printer
[ ] Learn about README.md format (image insertion & style)
[ ] Write an article about this

Connections
===========

Depending on the board the i2c pins are:

| Board         |   I2C / TWI pins                 |
| :--:          | :--:                             |
| Uno, Ethernet |   A4 (SDA), A5 (SCL)             |
| Mega2560      |   20 (SDA), 21 (SCL)             |
| Leonardo      |   2 (SDA), 3 (SCL)               |
| Due           |   20 (SDA), 21 (SCL), SDA1, SCL1 |


Address
=======

1010 + A2 + A1 + A0

A0 = 1
A1 = 1
A2 = 0

1010011 = 83

A0, A1, A2 can be not used if the eeprom has more than 2K memory.


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

Print the data with:

	$ cat dump.txt > data; echo "plot 'data'; pause mouse" | gnuplot

Save the data in a binary file dump.bin:

	$ head -n 256 dump.txt | xargs printf "%02x\n" | xxd -c 1 -p -r > dump.bin
	$ xxd dump.bin
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

Transform the binary data into a C array:

	$ xxd -i dump.bin | tee dump_bin.h
	unsigned char dump_bin[] = {
	  0x20, 0x00, 0x01, 0x03, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0x15, 0x04, 0x4d, 0x47, 0x27, 0x00, 0x18, 0x82,
	  0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x01, 0x01, 0x58, 0x30, 0x32, 0x35,
	  0x4d, 0x34, 0x33, 0x31, 0x35, 0x36, 0x36, 0x20, 0x00, 0x45, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x01, 0x06, 0x20, 0x00, 0x01, 0x01, 0x20, 0x00, 0x01, 0x01,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x71, 0x5d, 0x10, 0x00, 0x14, 0x27,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	  0xff, 0xff, 0xff, 0xff
	};
	unsigned int dump_bin_len = 256;


USB packet capture
==================

Here is an example of USB command sent by the proprietary windows
driver to the printer (captured with tcpdump when running windows
inside qemu):

	%-12345X@PJL
	@PJL SET TIMESTAMP=2015/09/14 21:15:14
	@PJL SET FILENAME=test - Notepad
	@PJL SET COMPRESS=JBIG
	@PJL SET USERNAME=IEUser
	@PJL SET COVER=OFF
	@PJL SET HOLD=OFF
	@PJL SET PAGESTATUS=START
	@PJL SET COPIES=1
	@PJL SET MEDIASOURCE=TRAY1
	@PJL SET MEDIATYPE=PLAINRECYCLE
	@PJL SET PAPER=LETTER
	@PJL SET PAPERWIDTH=5100
	@PJL SET PAPERLENGTH=6600
	@PJL SET RESOLUTION=600
	@PJL SET IMAGELEN=691
	[... image data ... ]
	@PJL SET DOTCOUNT=10745
	@PJL SET PAGESTATUS=END
	@PJL EOJ
	%-12345X


Data hypothesis
===============

From what the dirver send, the print has access to:

* the number of page
* the number of 'dot' per page

The number of 'dot' per page can be sent to zero. On my computer i updated
the driver to send the proper value. This is probably why my tonner run out
ink so quickly: during my experiments i probably sent the wrong values.

The eeprom is a simple data storage. The printer might wants to:

* store the number of printed pages and/or the number of printed dot
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
		  ^^^^ ^^^^ ^^^^ ^^^^      ^^^^ ^^^^ ^^^^
		  header                   printer crash if 0x00

	00000010: 1504 4d47 2700 1882 0000 0000 2000 0101  ..MG'....... ...
		  ^^^^           ^^^^
		  increase between two prints

	00000020: 5830 3235 4d34 3331 3536 3620 0045 0000  X025M431566 .E..
		  ^^^^ ^^^^ ^^^^ ^^^^ ^^^^ ^^^^
                  written by printer


	00000030: 0000 0000 0000 0000 0000 0106 0000 0000  ................
		                           ^^^^
		                           written by printer

	00000040: 0000 0107 0000 0000 0000 0000 0000 0000  ................
		       ^^^^
		       increase between two prints

	00000050: 0000 0000 0000 0106 2000 0101 2000 0101  ........ ... ...
		                      ^^^^ ^^^^
		                      written by printer

	00000060: 0000 0000 0000 0000 0106 0000 0000 0000  ................
		                      ^^^^
		                      increase between two prints

	00000070: 000e 715d 1000 1427 0000 0000 0000 0000  ..q]...'........
		       ^^^^ ^^^^ ^^^^
		       increase between two prints

	00000080: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000090: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000a0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000b0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000c0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000d0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000e0: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	000000f0: ffff ffff ffff ffff ffff ffff ffff ffff  ................

From another ricoh printer:

Black:

	00000000: a800 0103 1201 01ff 6400 3430 3735 3433  ........d.407543
	00000010: 1409 4142 1600 1626 ffff ffff ffff ffff  ..AB...&........
	00000020: ffff ffff ffff ffff ffff ffff 6400 0000  ............d...
	00000030: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000040: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000050: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000060: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000070: ffff ffff ffff ffff 0000 0000 0000 0000  ................

Magenta:

	00000000: a800 0103 0e03 01ff 6400 3430 3735 3435  ........d.407545
	00000010: 1501 4142 1800 0372 ffff ffff ffff ffff  ..AB...r........
	00000020: ffff ffff ffff ffff ffff ffff 6400 0000  ............d...
	00000030: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000040: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000050: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000060: ffff ffff ffff ffff ffff ffff ffff ffff  ................
	00000070: ffff ffff ffff ffff 0000 0000 0000 0000  ................

Same pattern:

	Magic + printer part number
	Counter + 4142 + Counter

=> We can try to reset the counter and hope for not double checks

Printer i2c commands
====================

Here is the sequence of read/write operations from the printer
captured by a logical analyser:

```sh
	$ sigrok-cli -i start_printer_failed.sr -P i2c:scl=2:sda=3,eeprom24xx
	Control code bits: 1010
	Address bit 2: 0
	Address bit 1: 1
	Address bit 0: 1
	R/W bit: write
	Control word
	Warning: Slave replied, but master aborted!
	Control code bits: 1010
	Address bit 2: 0
	Address bit 1: 1
	Address bit 0: 1
	R/W bit: write
	Control word
	Control code bits: 1010
	Address bit 2: 0
	Address bit 1: 1
	Address bit 0: 1
	R/W bit: read
	Control word
	Word address byte: 00
	Sequential random read (addr=00, 128 bytes): 20 00 01 03 01 01
	03 00 00 00 FF FF FF FF FF FF 15 04 4D 47 27 00 18 82 00 00 00 00 20
	00 01 01 58 30 32 35 4D 34 33 31 35 36 36 20 00 45 00 00 00 00 00 00
	00 00 00 00 00 00 01 06 00 00 00 00 00 00 01 07 00 00 00 00 00 00 00
	00 00 00 00 00 00 00 00 00 00 00 01 06 20 00 01 01 20 00 01 01 00 00
	00 00 00 00 00 00 01 06 00 00 00 00 00 00 00 0E 73 51 10 00 14 27 00
	00 00 00 00 00 00 00
	Control code bits: 1010
	Address bit 2: 0
	Address bit 1: 1
	Address bit 0: 1
	R/W bit: write
	Control word
	Word address byte: 76
	Word address
	Data byte 76: 14
	Data
	Byte write (addr=76, 1 byte): 14
	Control code bits: 1010
	Address bit 2: 0
	Address bit 1: 1
	Address bit 0: 1
	R/W bit: write
	Control word
	Word address byte: 70
	Word address
	Data byte 70: 00
	Data byte 71: 0E
	Data byte 72: 75
	Data byte 73: 45
	Data
	Page write (addr=70, 4 bytes): 00 0E 75 45
	Control code bits: 1010
	Address bit 2: 0
	Address bit 1: 1
	Address bit 0: 1
	R/W bit: write
	Control word
	Word address byte: 70
	Word address
	Data byte 70: 00
	Data byte 71: 0E
	Data byte 72: 77
	Data byte 73: 39
	Data
	Page write (addr=70, 4 bytes): 00 0E 77 39
	Control code bits: 1010
	Address bit 2: 0
	Address bit 1: 1
	Address bit 0: 1
	R/W bit: write
	Control word
	Word address byte: 77
	Word address
	Data byte 77: 5A
	Data
	Byte write (addr=77, 1 byte): 5A
	Control code bits: 1010
	Address bit 2: 0
	Address bit 1: 1
	Address bit 0: 1
	R/W bit: write
	Control word
	Word address byte: 70
	Word address
	Data byte 70: 00
	Data byte 71: 0E
	Data byte 72: 77
	Data byte 73: 8D
	Data
	Page write (addr=70, 4 bytes): 00 0E 77 8D
```

One sequential read of the content of the eeprom, followed by a
sequence of write operations: 4 byte at 0x70, 1 byte at 0x76.

This the printer read all the eeprom, it is difficult to figure out
which address hold which information.


Read 0x0 to 0xFF
Write 0x76: 64
Write 0x70: 00 00 01 F4
Write 0x70: 00 00 03 E8
Write 0x77: 33
Write 0x70: 00 00 04 C3

