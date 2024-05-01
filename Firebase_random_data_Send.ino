
#include <Arduino.h>
#include <WiFi.h>
// #include <Firebase_ESP_Client.h>
#include <FirebaseESP32.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "realme 10 Pro+ 5G"
#define WIFI_PASSWORD "Nahipata"


#define API_KEY "AIzaSyD27kAyt0pJ91cQIMCo37ryZeCWfoGa3OY"
#define USER_EMAIL "sharmadipanshu1432@gmail.com"
#define USER_PASSWORD "12345678";
#define DATABASE_URL "https://gps-data-f61e0-default-rtdb.asia-southeast1.firebasedatabase.app/"


FirebaseData firebaseData;
FirebaseJson json;
FirebaseAuth auth;
FirebaseConfig config;

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


void setup(){
  Serial.begin(115200);
  initWiFi();

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

}

void loop(){
 int Sdata = random(0,123);
 int Mdata = random(1,10);
 int Tdata = random(2,10);
 Serial.println(Sdata);

 delay(100);
 

  

    json.set("/LSET_VOL", Sdata);
    json.set("/Farm_VOL", Mdata);
    json.set("/Lora_VOL", Tdata);
    delay(1000);
    Firebase.updateNode(firebaseData,"/Sensor",json);
    // json.set(humPath.c_str(), String(bme.readHumidity()));
    // json.set(presPath.c_str(), String(bme.readPressure()/100.0F));
    // json.set(timePath, String(timestamp));
    // Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  
}