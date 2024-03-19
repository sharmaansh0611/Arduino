#include <SoftwareSerial.h>

SoftwareSerial gsmSerial(4, 5); 

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600); 

  delay(3000); 

  // Set up GSM module
  gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); 
  delay(1000);

  Serial.println("Setup complete.");
}

void loop() {
  // Check for incoming data from GSM module
  // if (gsmSerial.available() > 0) {
  //   String receivedMsg = gsmSerial.readString();
  //   Serial.println("Received message: " + receivedMsg);
  // }

  // Check for user input from serial monitor
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 's') {
      sendSMS();
    }
  }
}

void sendSMS() {
  Serial.println("Sending SMS...");

  
  gsmSerial.println("AT+CMGS=\"+918955393300\""); 
  delay(1000);
  gsmSerial.print("Hello"); 
  delay(1000);
  gsmSerial.println((char)26); 
  delay(1000);

  Serial.println("SMS sent.");
}
