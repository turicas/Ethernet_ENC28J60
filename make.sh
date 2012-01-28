#!/bin/bash

PORT=/dev/ttyACM0
ARDUINO_DIR=/home/alvaro/arduino-1.0
BOARD_TYPE=arduino
BAUD_RATE=115200
ENC28J60=/home/alvaro/projects/Ethernet_ENC28J60

ARDUINO_CORE=$ARDUINO_DIR/hardware/arduino/cores/arduino
INCLUDE="-I. -I$ARDUINO_DIR/hardware/arduino/cores/arduino -I$ENC28J60 -I$ENC28J60/utility \
	-I$ARDUINO_DIR/hardware/arduino/variants/standard"
TMP_DIR=/tmp/build_arduino
MCU=atmega328p
DF_CPU=16000000
CC=/usr/bin/avr-gcc
CPP=/usr/bin/avr-g++
AVR_OBJCOPY=/usr/bin/avr-objcopy
AVRDUDE=/usr/bin/avrdude
CC_FLAGS="-g -Os -w -Wall -ffunction-sections -fdata-sections -fno-exceptions -std=gnu99"
CPP_FLAGS="-g -Os -w -Wall -ffunction-sections -fdata-sections -fno-exceptions"
AVRDUDE_CONF=/etc/avrdude.conf
SKETCH_DESTINATION="$TMP_DIR/$(basename $SKETCH_NAME)"

compile() {
    rm -rf "$TMP_DIR"
    mkdir $TMP_DIR
    echo '#include "Arduino.h"' > "$SKETCH_DESTINATION.cpp"
    cat $SKETCH_NAME >> "$SKETCH_DESTINATION.cpp"

    $CPP -c -mmcu=$MCU -DF_CPU=$DF_CPU $INCLUDE $CPP_FLAGS "$SKETCH_DESTINATION.cpp" -o "$SKETCH_DESTINATION.o"
    for file in $(ls $ARDUINO_CORE/*.c); do
        o_file=$(basename $file | sed 's/\.c$/.o/')
        $CC -c -mmcu=$MCU -DF_CPU=$DF_CPU $INCLUDE $CC_FLAGS $file -o $TMP_DIR/$o_file
    done

    for file in $(ls $ARDUINO_CORE/*.cpp); do
        o_file=$(basename $file | sed 's/\.cpp$/.o/')
        $CPP -c -mmcu=$MCU -DF_CPU=$DF_CPU $INCLUDE $CPP_FLAGS $file -o $TMP_DIR/$o_file
    done

    $CC -c -mmcu=$MCU -DF_CPU=$DF_CPU $INCLUDE $CC_FLAGS $ENC28J60/utility/socket.c -o $TMP_DIR/socket.o
    $CC -c -mmcu=$MCU -DF_CPU=$DF_CPU $INCLUDE $CC_FLAGS $ENC28J60/utility/ip_arp_udp_tcp.c -o $TMP_DIR/ip_arp_udp_tcp.o
    $CC -c -mmcu=$MCU -DF_CPU=$DF_CPU $INCLUDE $CC_FLAGS $ENC28J60/utility/enc28j60.c -o $TMP_DIR/enc28j60.o
    $CPP -c -mmcu=$MCU -DF_CPU=$DF_CPU $INCLUDE $CPP_FLAGS $ENC28J60/Ethernet.cpp -o $TMP_DIR/Ethernet.o
    $CPP -c -mmcu=$MCU -DF_CPU=$DF_CPU $INCLUDE $CPP_FLAGS $ENC28J60/Server.cpp -o $TMP_DIR/Server.o
    $CPP -c -mmcu=$MCU -DF_CPU=$DF_CPU $INCLUDE $CPP_FLAGS $ENC28J60/Client.cpp -o $TMP_DIR/Client.o

    $CC -mmcu=$MCU -lm -Wl,--gc-sections -Os -o "$SKETCH_DESTINATION.elf" $TMP_DIR/*.o
    $AVR_OBJCOPY -O ihex -R .eeprom "$SKETCH_DESTINATION.elf" "$SKETCH_DESTINATION.hex"
}

reset() {
    stty --file $PORT hupcl
    sleep 0.1
    stty --file $PORT -hupcl
}

upload() {
    $AVRDUDE -q -V -p $MCU -C $AVRDUDE_CONF -c $BOARD_TYPE -b $BAUD_RATE -P $PORT -U flash:w:$SKETCH_DESTINATION.hex:i
}

compile
reset
upload
