# Eddie

This is Eddie. He is an arduino based soil, humidity and temperature sensor 
which transmits his readings to a raspberry pi node which relays the readings
to a [thingspeak channel](https://thingspeak.com/channels/92463).

Eddie is deployed in our greenhouse and powered by a 3.7v lithium ion battery
which is charged by a [cheap solar panel](http://www.amazon.co.uk/gp/product/B00EU6XZLQ).
Two DHT22 sensors are used, one for inside the greenhouse and one which is outside. The
project uses nRF24L01+ modules to transmit reading. Our setup uses the more powerful
nRF24L01+ with PA and LNA on the raspberry pi side to increase the transmission range.

There is also a [freeboard dashboard](http://freeboard.io/board/vIWE1d) of the readings.

## Building the Arduino Sensor

The [arduino code](sensor.ino) makes use of some external libraries. These will 
need be installed in `~/sketchbook/libraries` before you can program the arduino
using the [Arduino IDE](https://www.arduino.cc/en/Main/Software).

The required libraries are:

- [DHT](https://github.com/adafruit/DHT-sensor-library)
- [LowPower](https://github.com/rocketscream/Low-Power/)
- [RF24](https://github.com/TMRh20/RF24.git)
- [RF24Network](https://github.com/TMRh20/RF24Network.git)

The wiring diagram below describes how the arduino side is wired up.

![Arduino Wiring](fritzing/arduino_bb.png)

## Building the Raspberry Pi Relay node

Wire up a nRF24L01+ module to the raspberry pi:

![Raspberry Pi Wiring](fritzing/raspberrypi_bb.png)

The `receiver` application makes use of some additional libraries. These will 
need to be installed on your raspberry pi before you can build the code.

The required libraries are:

- [RF24](https://github.com/TMRh20/RF24.git)
- [RF24Network](https://github.com/TMRh20/RF24Network.git)
- [libcurl](https://curl.haxx.se/libcurl/)

  This can be installed with apt:

        apt-get install libcurl4-gnutls-dev

Having installed the prerequisite libraries:

    make

You can now run the receiver application by

    THINGSPEAK_API_KEY=Y0U4_AP1_K3Y ./receiver

### Running on startup

You can get the application to run on boot of your raspberry pi by adding a line in `/etc/rc.local`.

Adjust the end of your `/etc/rc.local` to look like:

    THINGSPEAK_API_KEY=Y0U4_AP1_K3Y /opt/eddie/receiver &
    exit 0

# Acknowledgements

- [Solar Powered Mini-Weather Station](http://forum.mysensors.org/topic/841/solar-powered-mini-weather-station) - A very similar project
- [Home Automation for Geeks](http://homeautomationforgeeks.com/rf24software.shtml) - wiring setup
- [@wshelley](https://github.com/wshelley) - Thanks to Wayne for his help and ideas
- [@joncooper65](https://github.com/joncooper65) - Thanks to Jon for introducing me to ThingSpeak and the name Eco-Dome (which became eddie)
