# This sample Makefile, explains how you can compile plain AVR C file.
#
# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

BOARD_TAG    = mega2560
MONITOR_PORT  = /dev/ttyACM0

ARDUINO_LIBS += Wire
ARDUINO_LIBS += SoftwareSerial

# include ../../Arduino.mk
include /usr/share/arduino/Arduino.mk

# !!! Important. You have to use make ispload to upload when using ISP programmer
