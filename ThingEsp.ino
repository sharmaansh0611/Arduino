#include <WiFi.h>
#include <ThingESP.h>
// ThingESP32 thing("username", "project_name", "credentials");
ThingESP32 thing("dipanshusharma", "hello", "module");

int LED = 4;
unsigned long previousMillis = 0;
const long INTERVAL = 6000;
int count =0;

void setup()
{
  Serial.begin(115200);

  pinMode(LED, OUTPUT);

  pinMode(4, INPUT_PULLUP);

  thing.SetWiFi("realme 10 Pro+ 5G", "Nahipata");

  thing.initDevice();

}



String HandleResponse(String query)
{

  if (query == "on") {
    digitalWrite(LED, 0);
    return "Done: LED Turned ON";
  }

  else if (query == "off") {
    digitalWrite(LED, 1);
    return "Done: LED Turned OFF";
  }

  else if (query == "Status")
    return digitalRead(LED) ? "LED is OFF" : "LED is ON";


  else return "Your query was invalid..";

}




void loop()
{

  if (digitalRead(4)==LOW) {
    count++;
    Serial.print(count);
  }

  delay(1000);


        if(count==3){
        String msg = digitalRead(LED) ? "LED is ON" : "LED is OFF";
        thing.sendMsg("+919622541073", msg);
        }

  thing.Handle();
}