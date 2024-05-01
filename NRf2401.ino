#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


SoftwareSerial gpsSerial(10, 11); // RX, TX pins for GPS module
TinyGPSPlus gps;

RF24 radio(8, 9); // CE, CSN pins for NRF24L01 module
const byte address[6] = "00001"; // Address for NRF24L01 communication

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW); // Set power level (optional)
  radio.stopListening(); // Set module to transmitter mode
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        
        sendGPSData(latitude, longitude); // Send data over NRF24L01
      }
    }
  }
}

void sendGPSData(float lat, float lng) {
  struct GPSData {
    float latitude;
    float longitude;
  };

  GPSData data;
  data.latitude = lat;
  data.longitude = lng;

  radio.write(&data, sizeof(data)); // Send GPS data via NRF24L01

  Serial.println("GPS data sent via NRF24L01");
}