#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <DHT.h>

DHT in_dht;
DHT ext_dht;
RF24 radio(7,8); 

RF24Network network(radio);

const uint16_t MY_ADDR = 01;
const uint16_t RASPBERRYPI_ADDR = 00;
const int SOIL_VCC = 9;
const int CYCLE = 16;
const int DHT_SLEEP = 6;

struct Payload {
  float in_temp;
  float in_hum;
  float ext_temp;
  float ext_hum;
  float voltage;
  unsigned int soil1;
  unsigned int soil2;
};

/**
 * Setup the radio, NRF24Network and the DHT22. We will broadcast sensor 
 * readings on frequency 108 (above Wifi routers)
 */
void setup(void) {
  SPI.begin();
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setAutoAck(1);
  radio.setRetries(15,15);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setCRCLength(RF24_CRC_8);
  network.begin(108, MY_ADDR);
  network.setup_watchdog(9); // 8 second cycle
  
  in_dht.setup(6);
  ext_dht.setup(5);
  pinMode(SOIL_VCC, OUTPUT);
}

/*
 * Read all the sensor values and store these in a payload struct.
 * The DHT22 sensors are contsantly powered, so the values can just be read.
 * However to extend the lifetime of the soil sensor, we will only power up to
 * take a reading.
 */
struct Payload read() {
  Payload data;
  data.in_temp  = in_dht.getTemperature();
  data.in_hum   = in_dht.getHumidity();
  data.ext_temp = ext_dht.getTemperature();
  data.ext_hum  = ext_dht.getHumidity();
  
  digitalWrite(SOIL_VCC, HIGH);
  delay(100); //Power up the soil sensor for 100 milliseconds before reading
  data.soil1 = analogRead(3);
  data.soil2 = analogRead(0);
  digitalWrite(SOIL_VCC, LOW);
  
  data.voltage = analogRead(2) * 4.3 / 1023;
  return data;
}

/**
 * Transmit a populated payload to the raspberry pi node.
 */
void send(Payload data) {
  network.update();
  RF24NetworkHeader header(RASPBERRYPI_ADDR);
  network.write(header,&data,sizeof(data));
}

/**
 * Reset the dht library timer. Since we are sleeping the arduino, millis() 
 * does not increase in real time.
 */
void reset() {  
  in_dht.resetTimer();
  ext_dht.resetTimer();
}

void loop() {
  Payload data = read();
  radio.powerUp();
  send(data);

  radio.powerDown();     
  network.sleepNode(off, 255); // Sleep with no chance of waking     
  reset(); // Reset the dht sensors if slept longer than DHT_SLEEP
}
