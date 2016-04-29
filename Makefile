ARCH=armv6zk
ifeq "$(shell uname -m)" "armv7l"
ARCH=armv7-a
endif

# Detect the Raspberry Pi from cpuinfo
#Count the matches for BCM2708 or BCM2709 in cpuinfo
RPI=$(shell cat /proc/cpuinfo | grep Hardware | grep -c BCM2708)
ifneq "${RPI}" "1"
RPI=$(shell cat /proc/cpuinfo | grep Hardware | grep -c BCM2709)
endif

ifeq "$(RPI)" "1"
# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-march=$(ARCH) -mtune=arm1176jzf-s -std=c++0x
endif

all: receiver

receiver: receiver.o
	g++ ${CCFLAGS} -Wall -I../ -lrf24-bcm -lrf24network -lcurl $@.cpp -o $@

clean:
	$(RM) receiver
