#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := Faikin
SUFFIX := $(shell components/ESP32-RevK/buildsuffix)

ifeq ($(wildcard /bin/csh),)
$(error	Please install /bin/csh or equivalent)
endif

all:	main/settings.h
	@echo Make: $(PROJECT_NAME)$(SUFFIX).bin
	@C:/Users/marce/esp/v5.4.1/esp-idf/tools/idf.py build
	@cp build/$(PROJECT_NAME).bin $(PROJECT_NAME)$(SUFFIX).bin
	@cp build/bootloader/bootloader.bin $(PROJECT_NAME)$(SUFFIX)-bootloader.bin
	@echo Done: $(PROJECT_NAME)$(SUFFIX).bin

beta:	
	-git pull
	-git submodule update --recursive
	-git commit -a
	@make set
	cp $(PROJECT_NAME)*.bin release/beta
	git commit -a -m Beta
	git push

issue:	
	-git pull
	-git commit -a
	cp -f release/beta/$(PROJECT_NAME)*.bin release
	git commit -a -m Release
	git push

main/settings.h:     components/ESP32-RevK/revk_settings main/settings.def components/ESP32-RevK/settings.def
	components/ESP32-RevK/revk_settings $^

# Alias target for compatibility with build scripts
settings.h: main/settings.h
	@true

components/ESP32-RevK/revk_settings: components/ESP32-RevK/revk_settings.c
	@set -e; \
	if gcc -O -o $@ $< -g -Wall --std=gnu99 -lpopt; then :; \
	else gcc -O -o $@ $< -g -Wall --std=gnu99 libpopt.a; fi

components/ESP32-RevK/idfmon: components/ESP32-RevK/idfmon.c
	make -C components/ESP32-RevK idfmon

# set:    main/settings.h c3 wroom1 wroom solo pico s3n8 s3
set:    main/settings.h s3

c3:
	components/ESP32-RevK/setbuildsuffix -C3
	@make

s3:
        components/ESP32-RevK/setbuildsuffix -S3-MINI-N4
        @make

s3r2:
        components/ESP32-RevK/setbuildsuffix -S3-MINI-N4-R2
        @make

s3n8:
	components/ESP32-RevK/setbuildsuffix -S3-MINI-N8
	@make

s2:
	components/ESP32-RevK/setbuildsuffix -S2
	@make

pico:
	components/ESP32-RevK/setbuildsuffix -S1-PICO
	@make

wroom1:
	components/ESP32-RevK/setbuildsuffix -S1-V1
	@make

wroom:
	components/ESP32-RevK/setbuildsuffix -S1
	@make

solo:
	components/ESP32-RevK/setbuildsuffix -S1-SOLO
	@make

flash:
	idf.py flash

monitor:
	idf.py monitor

clean:
	idf.py clean

menuconfig:
	idf.py menuconfig

pull:
	git pull
	git submodule update --recursive

update:
	-git pull
	-git commit -a
	git submodule update --init --recursive --remote
	-git commit -a -m "Library update"
	-git push

# Program the FTDI
ftdi: ftdizap/ftdizap
	./ftdizap/ftdizap --serial="RevK" --description="Faikin" --cbus2-mode=17 --self-powered=1

ifeq ($(shell uname),Darwin)
INCLUDES=-I/usr/local/include/
LIBS=-L/usr/local/Cellar/popt/1.18/lib/
else
LIBS=
INCLUDES=
endif

SQLlib/sqllib.o: SQLlib/sqllib.c
	make -C SQLlib
AXL/axl.o: AXL/axl.c
	make -C AXL
AJL/ajl.o: AJL/ajl.c
	make -C AJL
