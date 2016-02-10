# This sample Makefile, explains how you can compile plain AVR C file.
#
# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

# ARCHITECTURE  = avr
BOARD_TAG    = mega2560
BOARD_SUB     = atmega2560

MONITOR_PORT  = /dev/ttyACM0

AVRDUDE_ARD_PROGRAMMER = wiring
AVRDUDE_ARD_BAUDRATE = 115200

ARDUINO_LIBS += SoftwareSerial
ARDUINO_LIBS += Wire

# ARDUINO_DIR = /home/ludo/src/arduino-1.6.7

# VARIANT = mega
MCU = atmega2560
F_CPU = 16000000L


# include /usr/share/arduino/Arduino.mk
include /home/ludo/src/Arduino-Makefile/Arduino.mk
