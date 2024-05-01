#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SIM800L.h>

SoftwareSerial gsmSerial(10, 11); // RX, TX pins for GSM module
TinyGPSPlus gps;

const char* apn = "your-apn"; // APN for your GSM network provider
const char* gprsUser = "";    // GPRS username (if required by your network)
const char* gprsPass = "";    // GPRS password (if required by your network)

SIM800L gsm;

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);

  // Initialize GSM module
  while (!gsm.init(gsmSerial)) {
    Serial.println("GSM initialization failed");
    delay(1000);
  }

  // Connect to GSM network
  while (!gsm.connectGPRS(apn, gprsUser, gprsPass)) {
    Serial.println("GSM network connection failed");
    delay(1000);
  }

  Serial.println("GSM initialized and connected");
}

void loop() {
  while (gsmSerial.available() > 0) {
    if (gps.encode(gsmSerial.read())) {
      if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();

        sendGPSData(latitude, longitude); // Send data over GSM
      }
    }
  }
}

void sendGPSData(float lat, float lng) {
  // Send GPS data using GSM module
  gsm.beginSMS("+1234567890"); // Replace with your recipient phone number
  gsm.print("Latitude: ");
  gsm.print(lat, 6);
  gsm.print(", Longitude: ");
  gsm.println(lng, 6);
  gsm.endSMS();

  Serial.println("GPS data sent via SMS");
}