# Eddie

This is Eddie. He is an arduino based soil, humidity and temperature sensor 
which transmits his readings to a raspberry pi node which relays the readings
to a [thingspeak channel](https://thingspeak.com/channels/92463).

Eddie is deployed in our greenhouse and powered by a 3.7v lithium ion battery
which is charged by a [cheap solar panel](http://www.amazon.co.uk/gp/product/B00EU6XZLQ)
using a [TP4056](http://www.amazon.co.uk/Cars-XCSOURCE%C2%AE-Lithium-Battery-Charging-TE106/dp/B00SR4FH4A).
Two DHT22 sensors are used, one for inside the greenhouse and one which is outside. The
project uses nRF24L01+ modules to transmit reading. Our setup uses the more powerful
nRF24L01+ with PA and LNA on the raspberry pi side to increase the transmission range.

There is also a [freeboard dashboard](http://freeboard.io/board/vIWE1d) of the readings.

## Building Eddie

The [arduino code](eddie/eddie.ino) makes use of some external libraries. These will 
need be installed in `~/sketchbook/libraries` before you can program the arduino
using the [Arduino IDE](https://www.arduino.cc/en/Main/Software).

The required libraries are:

- [DHT](https://github.com/markruys/arduino-DHT)
- [RF24](https://github.com/TMRh20/RF24.git)
- [RF24Network](https://github.com/TMRh20/RF24Network.git)

**The RF24Network needs to be customised to enable sleep mode. See `ENABLE_SLEEP_MODE` in `RF24Network_config.h`**

The wiring diagram below describes how the eddie side is wired up.

![Eddie Wiring](images/eddie_bb.png)

### Mini Eddie

Eddie acts as a relay when he has enough charge. This allows mini eddie nodes to transmit
back to the raspberry pi via eddie. Mini Eddie nodes are much smaller and less power 
hungry than eddie. They only record soil moisture and fit snuggly in to the top of a
solar light sensor.

![Mini Eddie](images/mini_eddie.jpg)

Below is the wiring for a mini eddie node. Powered by a single 1.2v solar recharged battery.

![Mini Eddie Wiring](images/mini_eddie_bb.png)

## Building the Raspberry Pi Relay node

Wire up a nRF24L01+ module to the raspberry pi:

![Raspberry Pi Wiring](images/raspberrypi_bb.png)

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

    EDDIE_API_KEY=Y0U4_AP1_K3Y MINI_EDDIE_API_KEY=An0THER_K3Y ./receiver

### Running on startup

You can get the application to run on boot of your raspberry pi by adding a line in `/etc/rc.local`.

Adjust the end of your `/etc/rc.local` to look like:

    EDDIE_API_KEY=Y0U4_AP1_K3Y MINI_EDDIE_API_KEY=An0THER_K3Y /opt/eddie/receiver &
    exit 0

# Acknowledgements

- [Solar Powered Mini-Weather Station](http://forum.mysensors.org/topic/841/solar-powered-mini-weather-station) - A very similar project
- [Home Automation for Geeks](http://homeautomationforgeeks.com/rf24software.shtml) - wiring setup
- [@wshelley](https://github.com/wshelley) - Thanks to Wayne for his help and ideas
- [@joncooper65](https://github.com/joncooper65) - Thanks to Jon for introducing me to ThingSpeak and the name Eco-Dome (which became eddie)
