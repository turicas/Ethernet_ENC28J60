###############################################################################
#                     Makefile for Arduino Duemilanove/Uno                    #
#                  http://github.com/turicas/arduinoMakefile                  #
#                                                                             #
#        This Makefile was adapted to compile Ethernet_ENC28J60 files         #
#                                                                             #
#                     Created by Álvaro Justen aka Turicas                    #
#             alvarojusten [no-spam_AT_] gmail [_DOT_no-spam] com             #
#                         http://twitter.com/turicas                          #
#                                                                             #
#                               License: GPLv2                                #
#                   http://www.gnu.org/licenses/gpl-2.0.html                  #
###############################################################################

#Sketch, board and IDE path configuration (in general change only this section)
# Sketch filename without .pde (should be in the same directory of Makefile)
SKETCH_NAME=WebServerDEBUG
# The port Arduino is connected
#  Uno, in GNU/linux: generally /dev/ttyACM0
#  Duemilanove, in GNU/linux: generally /dev/ttyUSB0
PORT=/dev/ttyACM0
# The path of Arduino IDE
ARDUINO_DIR=/home/alvaro/arduino-0022
# Boardy type: use "arduino" for Uno or "skt500v1" for Duemilanove
BOARD_TYPE=arduino
# Baud-rate: use "115200" for Uno or "19200" for Duemilanove
BAUD_RATE=115200
ENC28J60=/home/alvaro/projetos/Ethernet_ENC28J60

#Compiler and uploader configuration
ARDUINO_CORE=$(ARDUINO_DIR)/hardware/arduino/cores/arduino
INCLUDE=-I. -I$(ARDUINO_DIR)/hardware/arduino/cores/arduino -I$(ENC28J60) -I$(ENC28J60)/utility
TMP_DIR=/tmp/build_arduino
MCU=atmega328p
DF_CPU=16000000
CC=/usr/bin/avr-gcc
CPP=/usr/bin/avr-g++
AVR_OBJCOPY=/usr/bin/avr-objcopy 
AVRDUDE=/usr/bin/avrdude
CC_FLAGS=-g -Os -w -Wall -ffunction-sections -fdata-sections -fno-exceptions -std=gnu99
CPP_FLAGS=-g -Os -w -Wall -ffunction-sections -fdata-sections -fno-exceptions
AVRDUDE_CONF=/etc/avrdude.conf


all:		clean compile upload

clean:
		@echo '*** Cleaning...'
		rm -rf "$(TMP_DIR)"


compile:
		@echo '*** Compiling...'
		mkdir $(TMP_DIR)
		echo '#include "WProgram.h"' > "$(TMP_DIR)/$(SKETCH_NAME).cpp"
		cat $(SKETCH_NAME).pde >> "$(TMP_DIR)/$(SKETCH_NAME).cpp"
		@#$(CPP) -MM -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) "$(TMP_DIR)/$(SKETCH_NAME).cpp" -MF "$(TMP_DIR)/$(SKETCH_NAME).d" -MT "$(TMP_DIR)/$(SKETCH_NAME).o"
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) "$(TMP_DIR)/$(SKETCH_NAME).cpp" -o "$(TMP_DIR)/$(SKETCH_NAME).o"
		
		@#Arduino core .c dependecies:
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ARDUINO_CORE)/pins_arduino.c -o $(TMP_DIR)/pins_arduino.o
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ARDUINO_CORE)/WInterrupts.c -o $(TMP_DIR)/WInterrupts.o
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ARDUINO_CORE)/wiring_analog.c -o $(TMP_DIR)/wiring_analog.o
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ARDUINO_CORE)/wiring.c -o $(TMP_DIR)/wiring.o
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ARDUINO_CORE)/wiring_digital.c -o $(TMP_DIR)/wiring_digital.o
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ARDUINO_CORE)/wiring_pulse.c -o $(TMP_DIR)/wiring_pulse.o
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ARDUINO_CORE)/wiring_shift.c -o $(TMP_DIR)/wiring_shift.o

		@#Arduino core .cpp dependecies:
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) $(ARDUINO_CORE)/HardwareSerial.cpp -o $(TMP_DIR)/HardwareSerial.o
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) $(ARDUINO_CORE)/main.cpp -o $(TMP_DIR)/main.o
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) $(ARDUINO_CORE)/Print.cpp -o $(TMP_DIR)/Print.o
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) $(ARDUINO_CORE)/Tone.cpp -o $(TMP_DIR)/Tone.o
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) $(ARDUINO_CORE)/WMath.cpp -o $(TMP_DIR)/WMath.o
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) $(ARDUINO_CORE)/WString.cpp -o $(TMP_DIR)/WString.o

		@#TODO: compile libraries here
		@#TODO: use .d files to track dependencies and compile them -> change .c by -MM and use -MF to generate .d


		@#Compiling Ethernet_ENC28J60-specific .c and .cpp files:
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ENC28J60)/utility/socket.c -o $(TMP_DIR)/socket.o
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ENC28J60)/utility/ip_arp_udp_tcp.c -o $(TMP_DIR)/ip_arp_udp_tcp.o
		$(CC) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CC_FLAGS) $(ENC28J60)/utility/enc28j60.c -o $(TMP_DIR)/enc28j60.o
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) $(ENC28J60)/Ethernet.cpp -o $(TMP_DIR)/Ethernet.o
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) $(ENC28J60)/Server.cpp -o $(TMP_DIR)/Server.o
		$(CPP) -c -mmcu=$(MCU) -DF_CPU=$(DF_CPU) $(INCLUDE) $(CPP_FLAGS) $(ENC28J60)/Client.cpp -o $(TMP_DIR)/Client.o

		$(CC) -mmcu=$(MCU) -lm -Wl,--gc-sections -Os -o $(TMP_DIR)/$(SKETCH_NAME).elf $(TMP_DIR)/*.o
		$(AVR_OBJCOPY) -O ihex -R .eeprom $(TMP_DIR)/$(SKETCH_NAME).elf $(TMP_DIR)/$(SKETCH_NAME).hex
		@echo '*** Compiled successfully! \o/'


reset:
		@echo '*** Resetting...'
		stty --file $(PORT) hupcl
		sleep 0.1
		stty --file $(PORT) -hupcl
		

upload:
		@echo '*** Uploading...'
		$(AVRDUDE) -q -V -p $(MCU) -C $(AVRDUDE_CONF) -c $(BOARD_TYPE) -b $(BAUD_RATE) -P $(PORT) -U flash:w:$(TMP_DIR)/$(SKETCH_NAME).hex:i
		@echo '*** Done - enjoy your sketch!'
