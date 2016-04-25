#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(7,8); 

RF24Network network(radio);

const uint16_t MY_ADDR = 011;
const uint16_t REC_ADDR = 00;
const int RADIO_VCC = 9;
const int SOIL_VCC = 4;

struct Payload {
  float voltage;
  unsigned int soil1;
};

/**
 * Setup the radio, NRF24Network and the DHT22. We will broadcast sensor 
 * readings on frequency 108 (above Wifi routers)
 */
void setup(void) {
  pinMode(RADIO_VCC, OUTPUT);
  pinMode(SOIL_VCC, OUTPUT);
  digitalWrite(RADIO_VCC, HIGH);

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
}

/*
 * Read all the sensor values and store these in a payload struct.
 * The DHT22 sensors are contsantly powered, so the values can just be read.
 * However to extend the lifetime of the soil sensor, we will only power up to
 * take a reading.
 */
struct Payload read() {
  Payload data;
  data.voltage  = analogRead(3) * 3.3 / 1023;
  //Power up the soil sensor for 100 milliseconds before reading
  digitalWrite(SOIL_VCC, HIGH);
  delay(100);
  data.soil1 = analogRead(2);
  digitalWrite(SOIL_VCC, LOW);
  return data;
}

/**
 * Transmit a populated payload to the raspberry pi node.
 */
void send(Payload data) {
  radio.powerUp();  
  RF24NetworkHeader header(REC_ADDR);
  network.write(header,&data,sizeof(data));
  radio.powerDown();
}

void loop() {
  send(read());
  network.sleepNode(75,255); //sleep for 10 mins and don't wake up
}

