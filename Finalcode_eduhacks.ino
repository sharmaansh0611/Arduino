#include <WiFi.h>
#include <HardwareSerial.h>
#include <FirebaseESP32.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <ThingESP.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <TinyGPS++.h>
#include<string.h>
#include "ThingSpeak.h"


// Variables
const int ENA = 33;
int speed; 
const int resetButtonPin = 21; 
bool resetButtonState = HIGH;
bool previousResetButtonState = HIGH;
bool reset_motor = true;
const int irPin = 18; 
const int buzzerpin = 19;
int irState = HIGH;
int previousIrState = HIGH;
int irCount = 0;
bool irCountIncreased = false;
bool sendGPS = false;
unsigned long irStartTime = 0;
String outputString;
int sleepCount =0;
// String sleep_count = "";
String Gps_Link = "";


WiFiClient client;

#define CHANNEL_ID 2502239
#define CHANNEL_API_KEY "BZ09NQYGDRO3OEN6"
#define GPS_SERIAL Serial2
#define WIFI_SSID "realme 10 Pro+ 5G"
#define WIFI_PASSWORD "Nahipata"
// String outputString;

float latitude;
float longitude;

SoftwareSerial mySerial(16,17); // RX, TX
SoftwareSerial gsmSerial(22,23); 
TinyGPS gps;

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);

#define API_KEY "AIzaSyD27kAyt0pJ91cQIMCo37ryZeCWfoGa3OY"
#define USER_EMAIL "dipanshusharmaim3371@gmail.com"
#define USER_PASSWORD "12345678";
#define DATABASE_URL "https://gps-data-f61e0-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData firebaseData;
FirebaseJson json;
FirebaseAuth auth;
FirebaseConfig config;
TinyGPSPlus gps1;

// ThingESP32 thing("username", "project_name", "credentials");
ThingESP32 thing("dipanshusharma", "hello", "module");





unsigned long previousMillis = 0;
const long INTERVAL = 6000;

  void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void setup()
{
  Serial.begin(9600);

  // thing.SetWiFi("rrrr", "Nahipata");
   initWiFi();
   gsmSerial.begin(9600);

  thing.initDevice();
  ThingSpeak.begin(client);

  mySerial.begin(9600);
   GPS_SERIAL.begin(9600, SERIAL_8N1, 16, 17);
   delay(1000);
     config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  firebaseData.setResponseSize(4096);

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  String uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);


  pinMode(irPin, INPUT);
  pinMode(buzzerpin, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(ENA, OUTPUT);
  analogWrite(ENA, 200);


  delay(3000);

   gsmSerial.println("AT");
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); 
  delay(1000);

  Serial.println("Setup complete.");
}



String HandleResponse(String query)
{

  if (query == "location") {
    // digitalWrite(LED, 0);
    return Gps_Link;
  }

  else if (query == "sleep data") {
    String sleep_count ="";
     sleep_count += "Driver slept ";
        sleep_count += String(sleepCount);
         sleep_count += " Times";
    return sleep_count;
  }



  else return "Your query was invalid..";

}




void loop()
{
  int COUNT_GPS =0;
   while (GPS_SERIAL.available() > 0) {
    String Gps_link;
    if (gps1.encode(GPS_SERIAL.read())) {
      if (gps1.location.isValid()) {
        // Get latitude and longitude
        Gps_link += "https://maps.google.com/maps?q=loc:";
         latitude = gps1.location.lat();
         Gps_link += String(latitude,6);
         Gps_link += ",";
         longitude = gps1.location.lng();
         Gps_link += String(longitude, 5);

         Gps_Link = Gps_link;


        
        // Print latitude and longitude
        Serial.print("Latitude: ");
        Serial.println(latitude, 6);
        Serial.print("Longitude: ");
        Serial.println(longitude, 6);
        json.set("/Latitude", latitude);
        json.set("/Longitude", longitude);
        // json.set("/Lora_VOL", Tdata);
        delay(1000);
        Firebase.updateNode(firebaseData,"/Sensor",json);
        delay(100);
        COUNT_GPS++;
        Gps_link ="";
      }
    }
    if(COUNT_GPS>=1)
    break;
  }

  // Read IR sensor state
  irState = digitalRead(irPin);
  String command = Serial.readStringUntil('\n');

  // Check if IR sensor state changed to LOW
  
  if (irState == LOW && previousIrState == HIGH) {
    irStartTime = millis(); 
    irCountIncreased = false;
  }

   // Check if IR sensor has been low for at least 2 seconds
  if (irState == LOW && millis() - irStartTime >= 20 && !irCountIncreased) {
    irCount++;
    sleepCount++;
    ThingSpeak.writeField(CHANNEL_ID, 3, sleepCount, CHANNEL_API_KEY );
    irCountIncreased = true;
    Serial.print("IR Count: ");
    Serial.println(irCount);
    Serial.print("sleep count :");
    Serial.println(sleepCount);
    analogWrite(buzzerpin, 255);
    if (reset_motor) {
    for (speed = 200; speed>=130; speed--) {
      analogWrite(ENA, speed);
      delay(10);
    }
    }
    
  }

  
  // Check if IR sensor state changed to HIGH
  if (irState == HIGH && previousIrState == LOW) {
    irStartTime = 0; // Reset start time when IR sensor goes high
    analogWrite(buzzerpin, 0);
     if (reset_motor) {
    for (speed = 130; speed<=200; speed++) {
      analogWrite(ENA, speed);
      delay(10);
    }
    }
  }

  resetButtonState = digitalRead(resetButtonPin);
  if (resetButtonState == LOW && previousResetButtonState == HIGH) {
    irCount = 0; 
    Serial.println("IR Count Reset");
    reset_motor =true;
    analogWrite(ENA, 200);
  }
  previousResetButtonState = resetButtonState;

  if (irCount >=3) {
  reset_motor =false;
  analogWrite(ENA, 0);
  }
    // Check if IR count is 3
  if (irCount == 3) {
    sendGPS = true;
    irCount++;
    Serial.println("Sending GPS data...");
  }

  if (command.equals("send")) {
  sendGPS = true;
  }



  // //       if(count==3){
  //       String msg = digitalRead(LED) ? "LED is ON" : "LED is OFF";
  //       thing.sendMsg("+918955393300", msg);
  //       }

  // thing.Handle();

  // if (Serial.available()) {

  //     String command = Serial.readStringUntil('\n');
  //     if (command.equals("send")) {
  //       bool newdata = false;
  //       while (1) {
  //         if (mySerial.available()) {
  //           char c = mySerial.read();
  //           //  Serial.print(c);  // uncomment to see raw GPS data
  //           if (gps.encode(c)) {
  //             newdata = true;
  //             break;  // uncomment to print new data immediately!
  //           }
  //         }
  //       }
  //       if (newdata) {
  //         // Serial.println("Acquired Data");
  //         Serial.println();
  //         gpsdump(gps);
  //        Serial.print(outputString);
  //        String msg = outputString;
  //        Serial.print(outputString);
  //       thing.sendMsg("+918955393300", msg);

          
  //         // Serial.println("-------------");
  //         Serial.println();
  //       }
  //     }
  // }


  // Check if GPS data needs to be sent
  if (sendGPS) {
     bool newdata = false;
        while (1) {
          if (mySerial.available()) {
            char c = mySerial.read();
            //  Serial.print(c);  // uncomment to see raw GPS data
            if (gps.encode(c)) {
              newdata = true;
              break;  // uncomment to print new data immediately!
            }
          }
        }
        if (newdata) {
          // Serial.println("Acquired Data");
          Serial.println();
          gpsdump(gps);
          Serial.print(outputString);
         String msg = outputString;
         Serial.print(outputString);
        thing.sendMsg("+918955393300", msg);
          // sendSMS();
          // delay(300);
          // // Serial.println("-------------");
          // sendSMS();
          // delay(300);
          // sendSMS();
          Serial.println();
          
        }
         sendGPS = false; // Reset sendGPS flag
  }

  previousIrState = irState;



  if (irCount>=3) {
   if (Serial.available() > 0) {
    // char c = Serial.read();
    if (irCount>=3) {
      sendSMS();
    }
  }
  }
     

thing.Handle();
}



void gpsdump(TinyGPS &gps)
{
  // long lat, lon;
  float flat, flon;
  // unsigned long age;
  // unsigned short sentences, failed;

  // gps.get_position(&lat, &lon);
  // Serial.print("Lat/Long(10^-5 deg): "); Serial.print(lat); Serial.print(", "); Serial.print(lon); 
 

  gps.f_get_position(&flat, &flon);
  // Serial.print("https://maps.google.com/maps?q=loc:"); printFloat(flat, 5); Serial.print(", "); printFloat(flon, 5);
  outputString = "";

  outputString += "https://maps.google.com/maps?q=loc:";
  outputString += String(flat, 5);
  outputString += ",";
  outputString += String(flon, 5);
  // Serial.print(outputString);
  // return outputString;

}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}


void sendSMS() {
  Serial.println("Sending SMS...");

  
  gsmSerial.println("AT+CMGS=\"+918955393300\""); 
  delay(1000);
  gsmSerial.print(outputString); 
  delay(1000);
  gsmSerial.println((char)26); 
  delay(1000);

  Serial.println("SMS sent.");
}
