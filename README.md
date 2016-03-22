
Here are some advices to reset your toner chip with an Arduino.

Introduction
============

Some printer toner comes with a small circuit like:

![Picture of toner](/images/sp112_toner.png)

In order to reuse this kind of toner, there are two steps:

1. refill the toner with ink (if needed)
2. reset the toner chip (or replace it)

While there is plenty of information explaining how to refill the
toner, little information is available to erase the toner chip.

Refilling the toner with ink is describe in great lenght in [this documents from uni-kit.com](www.uni-kit.com/pdf/tonerrefillinstructions.pdf).

This document deals with the second part: how to analyse the chip and
reset it.

It took me a while to get everything setup and to have my toner chip
reset so i would like to share this process in order to help other to
do the same on their printer.

We will go step-by-step to understand the problem, analyse the
circuit, read the chip memory and write it back so the toner can
function again.

For more information about why manufaturer include those chips, read
the [about page](/ABOUT.md).

![Picture of the font circuit](/images/front_circuit.jpg)

Step 0: the problem
===================

Your computer talks to your printer via a USB link (or maybe through
wifi). The printer itself communicate with the toner chip via an I2C
or a SPI bus.

	+------------+           +-----------+            +-------------+
	|    Host    |    USB    |           |    I2C     |    toner    |
	|  computer  | <-------> |  Printer  | <--------> |    chip     |
	|            |           |           |            |             |
	+------------+           +-----------+            +-------------+

So what we will do is to connect our Arduino directly to the toner
chip like this:

	+-----------+          +-----------+
	|           |   I2C    |   toner   |
	|  Arduino  | <------> |   chip    |
	|           |          |           |
	+-----------+          +-----------+


IC2 buses are very common on embedded systems. For example,
smartphones use them to connect the touchscreen or the motion sensor
to the main processor chip. There is plenty of documentation, i like
[this one from saleae](http://support.saleae.com/hc/en-us/articles/200730905-Learn-I2C-Inter-Integrated-Circuit).

The full specification is avavailable at: http://www.i2c-bus.org/

For your particalar printer, it might be an SPI bus instead of an I2C
bus. But this does not really matter: the analysis procedure is the
same.

Step 1: the circuit
===================

The first step is to analyse the circuit. Here you should gather as
much information as you can:

* Read the part number and search it on the Internet.
* Search if other people have shared information about your printer.

In my case, the chip looked like an EEPROM memory. This is was
confirmed by two blogs discussing other model Ricoh printers:

* http://www.mikrocontroller.net/topic/369267
* https://esdblog.org/ricoh-sp-c250dn-laser-printer-toner-hack/

I still could not find the exact EERPOM model, but it is of the 24xxx
family.
 
![Front chip](/images/front_circuit.png)
![Front chip](/images/back_circuit.png)

Your particular circuit will be different from this one.

If you have absolutly no idea, jump to the section: "Bonus 1: snif the
I2C commands" at the end of this page.

The rest of the tutorial is about how to read and write this EEPROM
memory.

Step 2: connect your arduino
============================

Depending on the board the I2C pins are:

| Board         |   I2C pins           |
| :--:          | :--:                 |
| Uno, Ethernet |   A4 (SDA), A5 (SCL) |
| Mega2560      |   20 (SDA), 21 (SCL) |
| Due           |   20 (SDA), 21 (SCL) |
| Leonardo      |   2 (SDA), 3 (SCL)   |

Then connect GND and VCC to 3.3V.


Step 3: find the I2C clock and address
======================================

To communicate on an I2C bus, we need to know the clock speed and the
address of the EEPROM.

If you know the EEPROM model from the circuit analysis, you can
read the datasheet and find the clock rate and address like this:

For example the [datasheet of the component FM24C02B](/datasheet/FM24C02B-04B-08B-16B.pdf)
indicates an operating clock of 1MHz at 3.3V.
The datasheet indicates how to calculate the address according to the
PIN A0, A1 and A2. In binary, the address is computed like this: ``1 0
1 0 A2 A1 A0``.

So if the configuration is:

	A0 = 1
	A1 = 1
	A2 = 0

The address is ``1 0 1 0 0 1 1`` (83).

Note: 1MHz seems to be the upper bound of the my Arduino Mega can reach.
So I just use 800 kHz, see the [full discussion](http://electronics.stackexchange.com/questions/29457/how-to-make-arduino-do-high-speed-i2c).

If you don't know the clock rate and the device address on the I2C
bus, you can scan all the possible I2C addresses at different clock
rate. The directory scanner have a sketch to do this. Here is the
output of the program execution on my Arduino:

	Arduino I2C Scanner - 0.1.06

	Scanmode:
		s = single scan
		c = continuous scan - 1 second delay
		q = quit continuous scan
		d = toggle latency delay between successful tests. 0 - 5 ms
	Output:
		p = toggle printAll - printFound.
		h = toggle header - noHeader.
		a = toggle address range, 0..127 - 8..120
	Speeds:
		0 = 50 - 800 Khz
		1 = 100 KHz only
		2 = 200 KHz only
		4 = 400 KHz only
		8 = 800 KHz only

		? = help - this page

	TIME	DEC	HEX		50	100	200	250	400	500	800	[KHz]
	---------------------------------------------------------------------------------------------
	2987	0	0x00		.	.	.	.	.	.	.
	2990	1	0x01		.	.	.	.	.	.	. 
	2992	2	0x02		.	.	.	.	.	.	. 
	2994	3	0x03		.	.	.	.	.	.	. 
	2997	4	0x04		.	.	.	.	.	.	. 
	2999	5	0x05		.	.	.	.	.	.	. 
	3002	6	0x06		.	.	.	.	.	.	. 
	3004	7	0x07		.	.	.	.	.	.	. 
	3007	8	0x08		.	.	.	.	.	.	. 
	3009	9	0x09		.	.	.	.	.	.	. 
	3012	10	0x0A		.	.	.	.	.	.	. 
	3014	11	0x0B		.	.	.	.	.	.	. 
	3017	12	0x0C		.	.	.	.	.	.	. 
	3020	13	0x0D		.	.	.	.	.	.	. 
	3022	14	0x0E		.	.	.	.	.	.	. 
	3025	15	0x0F		.	.	.	.	.	.	. 
	3027	16	0x10		.	.	.	.	.	.	. 
	3031	17	0x11		.	.	.	.	.	.	. 
	3034	18	0x12		.	.	.	.	.	.	. 
	3036	19	0x13		.	.	.	.	.	.	. 
	3039	20	0x14		.	.	.	.	.	.	. 
	3041	21	0x15		.	.	.	.	.	.	. 
	3044	22	0x16		.	.	.	.	.	.	. 
	3046	23	0x17		.	.	.	.	.	.	. 
	3049	24	0x18		.	.	.	.	.	.	. 
	3052	25	0x19		.	.	.	.	.	.	. 
	3054	26	0x1A		.	.	.	.	.	.	. 
	3057	27	0x1B		.	.	.	.	.	.	. 
	3059	28	0x1C		.	.	.	.	.	.	. 
	3062	29	0x1D		.	.	.	.	.	.	. 
	3064	30	0x1E		.	.	.	.	.	.	. 
	3067	31	0x1F		.	.	.	.	.	.	. 
	3070	32	0x20		.	.	.	.	.	.	. 
	3073	33	0x21		.	.	.	.	.	.	. 
	3076	34	0x22		.	.	.	.	.	.	. 
	3078	35	0x23		.	.	.	.	.	.	. 
	3081	36	0x24		.	.	.	.	.	.	. 
	3083	37	0x25		.	.	.	.	.	.	. 
	3086	38	0x26		.	.	.	.	.	.	. 
	3089	39	0x27		.	.	.	.	.	.	. 
	3091	40	0x28		.	.	.	.	.	.	. 
	3094	41	0x29		.	.	.	.	.	.	. 
	3096	42	0x2A		.	.	.	.	.	.	. 
	3099	43	0x2B		.	.	.	.	.	.	. 
	3101	44	0x2C		.	.	.	.	.	.	. 
	3104	45	0x2D		.	.	.	.	.	.	. 
	3107	46	0x2E		.	.	.	.	.	.	. 
	3109	47	0x2F		.	.	.	.	.	.	. 
	3112	48	0x30		.	.	.	.	.	.	. 
	3115	49	0x31		.	.	.	.	.	.	. 
	3118	50	0x32		.	.	.	.	.	.	. 
	3120	51	0x33		.	.	.	.	.	.	. 
	3123	52	0x34		.	.	.	.	.	.	. 
	3126	53	0x35		.	.	.	.	.	.	. 
	3128	54	0x36		.	.	.	.	.	.	. 
	3131	55	0x37		.	.	.	.	.	.	. 
	3133	56	0x38		.	.	.	.	.	.	. 
	3136	57	0x39		.	.	.	.	.	.	. 
	3138	58	0x3A		.	.	.	.	.	.	. 
	3141	59	0x3B		.	.	.	.	.	.	. 
	3144	60	0x3C		.	.	.	.	.	.	. 
	3146	61	0x3D		.	.	.	.	.	.	. 
	3149	62	0x3E		.	.	.	.	.	.	. 
	3151	63	0x3F		.	.	.	.	.	.	. 
	3154	64	0x40		.	.	.	.	.	.	. 
	3156	65	0x41		.	.	.	.	.	.	. 
	3160	66	0x42		.	.	.	.	.	.	. 
	3163	67	0x43		.	.	.	.	.	.	. 
	3165	68	0x44		.	.	.	.	.	.	. 
	3168	69	0x45		.	.	.	.	.	.	. 
	3170	70	0x46		.	.	.	.	.	.	. 
	3173	71	0x47		.	.	.	.	.	.	. 
	3175	72	0x48		.	.	.	.	.	.	. 
	3178	73	0x49		.	.	.	.	.	.	. 
	3181	74	0x4A		.	.	.	.	.	.	. 
	3183	75	0x4B		.	.	.	.	.	.	. 
	3186	76	0x4C		.	.	.	.	.	.	. 
	3188	77	0x4D		.	.	.	.	.	.	. 
	3191	78	0x4E		.	.	.	.	.	.	. 
	3193	79	0x4F		.	.	.	.	.	.	. 
	3196	80	0x50		.	.	.	.	.	.	. 
	3200	81	0x51		.	.	.	.	.	.	. 
	3202	82	0x52		.	.	.	.	.	.	. 
	3205	83	0x53		V	V	V	V	V	V	V 
	3207	84	0x54		.	.	.	.	.	.	. 
	3210	85	0x55		.	.	.	.	.	.	. 
	3212	86	0x56		.	.	.	.	.	.	. 
	3215	87	0x57		.	.	.	.	.	.	. 
	3218	88	0x58		.	.	.	.	.	.	. 
	3220	89	0x59		.	.	.	.	.	.	. 
	3223	90	0x5A		.	.	.	.	.	.	. 
	3225	91	0x5B		.	.	.	.	.	.	. 
	3228	92	0x5C		.	.	.	.	.	.	. 
	3231	93	0x5D		.	.	.	.	.	.	. 
	3233	94	0x5E		.	.	.	.	.	.	. 
	3236	95	0x5F		.	.	.	.	.	.	. 
	3238	96	0x60		.	.	.	.	.	.	. 
	3241	97	0x61		.	.	.	.	.	.	. 
	3244	98	0x62		.	.	.	.	.	.	. 
	3247	99	0x63		.	.	.	.	.	.	. 
	3250	100	0x64		.	.	.	.	.	.	. 
	3252	101	0x65		.	.	.	.	.	.	. 
	3255	102	0x66		.	.	.	.	.	.	. 
	3257	103	0x67		.	.	.	.	.	.	. 
	3260	104	0x68		.	.	.	.	.	.	. 
	3263	105	0x69		.	.	.	.	.	.	. 
	3265	106	0x6A		.	.	.	.	.	.	. 
	3268	107	0x6B		.	.	.	.	.	.	. 
	3271	108	0x6C		.	.	.	.	.	.	. 
	3273	109	0x6D		.	.	.	.	.	.	. 
	3276	110	0x6E		.	.	.	.	.	.	. 
	3279	111	0x6F		.	.	.	.	.	.	. 
	3281	112	0x70		.	.	.	.	.	.	. 
	3284	113	0x71		.	.	.	.	.	.	. 
	3288	114	0x72		.	.	.	.	.	.	. 
	3290	115	0x73		.	.	.	.	.	.	. 
	3293	116	0x74		.	.	.	.	.	.	. 
	3296	117	0x75		.	.	.	.	.	.	. 
	3298	118	0x76		.	.	.	.	.	.	. 
	3301	119	0x77		.	.	.	.	.	.	. 
	3304	120	0x78		.	.	.	.	.	.	. 
	3306	121	0x79		.	.	.	.	.	.	. 
	3309	122	0x7A		.	.	.	.	.	.	. 
	3312	123	0x7B		.	.	.	.	.	.	. 
	3314	124	0x7C		.	.	.	.	.	.	. 
	3317	125	0x7D		.	.	.	.	.	.	. 
	3320	126	0x7E		.	.	.	.	.	.	. 
	3322	127	0x7F		.	.	.	.	.	.	. 

	1 device find in 347 milliseconds.


From here, we know the device address on the I2C bus is 83 (0x53) and
the operating clock is anything between 50 kHz and 800 kHz.


Step 4: reading the EEPROM
==========================

Since we know how to communicate with the chip, let's read the content
of the memory. For 24xxx EEPROM, the [datasheet for FM24C02B](/datasheet/FM24C02B-04B-08B-16B.pdf) explains how to complete a read operation:

	master send start condition
	master send eeprom address + read bit
	master send data address
	master send start condition
	master send eeprom address + read bit
	device respond with data
	master send stop condition

	STOP condition mandatory between writes.
	Write cycle: 5 ms.

The function "printRandomAddress" of the Reset.ino sketch implements
this random read operation. Once the read operation works, you can
print each bytes of the EEPROM one by one and save them in a file.

```sh
	$ head dump.txt
	0x20
	0x0
	0x1
	0x3
	0x1
	0x1
	0x3
	0x0
	0x0
	0x0
```

Next I converted the text file into a binary format. I found binary
format easier to analyze with xxd:

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

Here we can see:

* there seems to be a header (0x0 to 0x0f),
* followed by some numbers (0x10 to 0x1f),
* followed by a string (X025M431566)
* followed by some sparse values

This does not make much sense. The next step is to figure out what
those values are for.

Step 5: understand the data
===========================

This is now the detective part : we need to understand the memory
layout.


From what the dirver send, the print has access to:

* the number of page
* the number of 'dot' per page

The number of 'dot' per page can be sent to zero. On my computer i updated
the driver to send the proper value. This is probably why my toner run out
ink so quickly: during my experiments i probably sent the wrong values.

The eeprom is a simple data storage. The printer might wants to:

* store the number of printed pages and/or the number of printed dot
* mark is as used by a particular printer to prevent second hand market
* mark the date of the first and last usage to make it out of date.
* the toner may have the information of the maximum 'dot' capacity (or
  a count down value).

Here is what could be the printer logical sequence:

* Power on printer
* Check if a toner is present on the I2C bus:
	* Check a device respond
	* Verifies the magic number
	* Check if the toner is compatible this printer model (sp112)
	* Check if associated with this particular printer
		* if not associated, write the printer PN into the toner
* Check if the toner is usable:
	* Check if the toner is broken
		* Read a broken information (ex: like an error code on eeprom)?
	* Check if the toner is too old:
		* Read the first date of usage
		* Read the last date of usage
	* Check if the toner capacity is exhausted:
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
			* if so mark the toner a broken (reach max pages)
		* Verify the dot counter is exhausted
			* if so mark the toner a broken (reach max dots)
		* Compare the date with the first usage
			* if no first usage, mark this date as the first usage
			* if first usage more than duration mark the toner as broken (too old)
		* Compare the date with the last usage
			* if last usage less than continous mark the toner as broken (too dry)
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
* if the toner needs to warm-up, hours/minutes/seconds is also required.

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


Step 6: try some changes
========================

Another advantage of the binary data: with xxd you can convert the
binary data into a C header file. This C header file can be included
in your program.

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


Step 7: share youf findings
===========================

Bonus 1: snif the I2C commands
=============================

One way to gain some insight about the memory layout, is to spy the
communication between the printer and the toner during the printing
processus.

In order to do so, we can use a logical analyser. This is a simple
equipement that will record the logical state of the wires.


	+------------+           +-----------+            +-------------+
	|    Host    |    USB    |           |    I2C     |    toner    |
	|  computer  | <-------> |  Printer  | <--------> |    chip     |
	|            |           |           |      ^     |             |
	+------------+           +-----------+      |     +-------------+
	                                            |
	                                        logical
	                                        analyser

This can be very useful to learn:

* the general properties of the communication (type of
  bus, clock speed, device address).
* what memory address the printer read and write

The main open source project to use logical analysers is
[Sigrok](http://sigrok.org). For the list of supported analysers
please refer to [supported hardware page](https://sigrok.org/wiki/Supported_hardware).

The following pictures explain the settings:

1. attach the wires to the circuit and put back the circuit on the
   toner in the printer.
2. connect the logical analyzer.

![Picture of the back circuit](/images/back_circuit.jpg)
![Picture of logical analyzer in the printer](/images/logical_analyser.jpg)

To capture the data transfert, you can use a graphical tool
like [Pulseview](https://sigrok.org/wiki/PulseView).

![Picture of Pulseview interface](/pulseview/capture_eeprom.png)

You can also use the command line tool called
[sigrok-cli](https://sigrok.org/wiki/Sigrok-cli) as demonstrated
bellow:


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

How to read this: one sequential read of the content of the eeprom,
followed by a sequence of write operations: 4 byte at 0x70, 1 byte at
0x76. This the printer read all the eeprom, it is difficult to figure
out which address hold which information.


Bonus 2: snif the USB packets
============================

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


Usage
=====

```sh
	$ make
	$ make upload
	$ picocom -b 115200 /dev/ttyACM0
```
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



Todo
====

- [x] Create arduino hello world
- [x] Read internal EEPROM
- [x] Draw the cricuit
- [x] Understand the cricuit
- [x] Try I2C clock at 400kHz and 1MHz
- [x] Scan for device => use MultiSpeedScanner
- [x] Analyse I2C trame with a logical analyser
- [x] Visualize I2C packets with pulseview
- [x] Read one EEPROM datasheet
- [x] Debug I2C addresses sent (1010001 and not 0101000) ~ frequence to high
- [x] Verifies the timming between read and write operations (5ms)
- [x] Find the exact EEPROM chip model
- [x] Find the EEPROM address (0x53)
- [x] Read the EPPROM chip
- [x] Order sp112 reset chip from internet
- [x] Analyse the EEPROM dump
- [x] Make a data hypothesis
- [x] Verify the write function
- [x] Dump a new reset chip
- [x] Write the EEPROM with a dump of a new reset chip
- [x] Test with the printer
- [ ] Learn about README.md format (image insertion & style)
- [ ] Write an article about this


<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.
