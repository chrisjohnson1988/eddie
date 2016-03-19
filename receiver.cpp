#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <curl/curl.h>
#include <sstream>

RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
RF24Network network(radio);

const uint16_t MY_ADDR = 00;
const uint16_t ARDUINO_ADDR = 01;

struct Payload {
  float in_temp;
  float in_hum;
  float out_temp;
  float out_hum;
  float voltage;
  uint16_t soil1;
  uint16_t soil2;
};

/**
 * Start looping listening to packets from the NRF24 radio. If one is received 
 * from the arduino sensor node then submit its values to thingspeak
 */
int main(int argc, char *argv[]) {
  if(char* api_key = std::getenv("THINGSPEAK_API_KEY")) {
    radio.begin();
    radio.enableDynamicPayloads();
    radio.setAutoAck(1);
    radio.setRetries(15,15);
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX);
    radio.setCRCLength(RF24_CRC_8);
    network.begin(108, MY_ADDR);
    radio.printDetails();
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl;
    curl = curl_easy_init();

    while(1) {
      network.update();
      while ( network.available() ) {
        RF24NetworkHeader header;
        Payload payload;
        network.read(header,&payload,sizeof(payload));

        if(header.from_node == ARDUINO_ADDR) {
          std::stringstream data;
          data << "https://api.thingspeak.com/update?"
               << "api_key=" << api_key          << "&"
               << "field1="  << payload.in_temp  << "&"
               << "field2="  << payload.in_hum   << "&"
               << "field3="  << payload.out_temp << "&"
               << "field4="  << payload.out_hum  << "&"
               << "field5="  << payload.voltage  << "&"
               << "field6="  << payload.soil1    << "&"
               << "field7="  << payload.soil2;

           curl_easy_setopt(curl, CURLOPT_URL, data.str().c_str());
           curl_easy_perform(curl);
           std::cout << " Packet Send\n";
         }
      }
      delay(2000);
    }
    curl_global_cleanup();
    return EXIT_SUCCESS;
  }
  std::cout << "Expected to find THINGSPEAK_API_KEY environment variable to be defined\n";
  return EXIT_FAILURE;
}

