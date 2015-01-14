#!/bin/sh
avrdude -p m168 -c usbtiny -U flash:w:main.hex
