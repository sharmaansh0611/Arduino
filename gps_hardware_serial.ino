#include <TinyGPS++.h>
#include <HardwareSerial.h>
#define GPS_RX_PIN 16  // Connect NEO-6M TX pin to GPIO 16 (RX2)
#define GPS_TX_PIN 17  // Connect NEO-6M RX pin to GPIO 17 (TX2)

// Define the serial connections for GPS module and ESP32
#define GPS_SERIAL Serial2
#define BAUD_RATE_GPS 9600

// Create a TinyGPS++ object to handle parsing GPS data
TinyGPSPlus gps;

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  
  // Initialize serial communication with the GPS module
  GPS_SERIAL.begin(BAUD_RATE_GPS, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

void loop() {
  // Read data from GPS module
  while (GPS_SERIAL.available() > 0) {
    if (gps.encode(GPS_SERIAL.read())) {
      if (gps.location.isValid()) {
        // Get latitude and longitude
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        
        // Print latitude and longitude
        Serial.print("Latitude: ");
        Serial.println(latitude, 6);
        Serial.print("Longitude: ");
        Serial.println(longitude, 6);
      }
    }
  }
}
