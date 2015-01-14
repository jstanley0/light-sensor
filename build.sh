#!/bin/sh
avr-gcc -Wall -Os -DF_CPU=8000000 -mmcu=atmega168 -c light-sensor.c -o main.o
avr-gcc -Wall -Os -DF_CPU=8000000 -mmcu=atmega168 -o main.elf main.o
rm -f main.hex
avr-objcopy -j .text -j .data -O ihex main.elf main.hex
avr-size --format=avr --mcu=atmega168 main.elf
