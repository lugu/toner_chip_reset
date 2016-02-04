# This sample Makefile, explains how you can compile plain AVR C file.
#
# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

BOARD_TAG    = mega2560
MONITOR_PORT  = /dev/ttyACM0

ARDUINO_LIBS += SoftwareSerial
ARDUINO_LIBS += Wire

include /usr/share/arduino/Arduino.mk
