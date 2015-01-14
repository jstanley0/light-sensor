DEVICE     = atmega168
CLOCK      = 8000000
PROGRAMMER = -c usbtiny -p m168
OBJECTS    = light-sensor.o
FUSES      = -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xf9:m


AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# symbolic targets:
all:	light-sensor.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:light-sensor.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID light-sensor.hex

clean:
	rm -f light-sensor.hex light-sensor.elf $(OBJECTS)

# file targets:
light-sensor.elf: $(OBJECTS)
	$(COMPILE) -o light-sensor.elf $(OBJECTS)

light-sensor.hex: light-sensor.elf
	rm -f light-sensor.hex
	avr-objcopy -j .text -j .data -O ihex light-sensor.elf light-sensor.hex
	avr-size --format=avr --mcu=$(DEVICE) light-sensor.elf
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	light-sensor.elf
	avr-objdump -d light-sensor.elf

cpp:
	$(COMPILE) -E light-sensor.c
