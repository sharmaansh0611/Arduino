#include <SoftwareSerial.h>
#include <TinyGPS.h>


SoftwareSerial mySerial(10,11); // RX, TX
TinyGPS gps;

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  
  delay(1000);

  
}
void loop() // run over and over
{

  // bool newdata = false;
  

  // // Every 5 seconds we print an update
  // while (1) {
  //   if (mySerial.available()) {
  //      char c = mySerial.read();
  //     //  Serial.print(c);  // uncomment to see raw GPS data
  //      if (gps.encode(c)) {
  //       newdata = true;
  //        break;  // uncomment to print new data immediately!
  //      }
  //    }
  // }

 
     
  
  // if (newdata) {
  //   Serial.println("Acquired Data");
  //   Serial.println("-------------");
  //   gpsdump(gps);
  //   // Serial.println("-------------");
  //   Serial.println();
  // }

  if (Serial.available()) {

      String command = Serial.readStringUntil('\n');
      if (command.equals("send")) {
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
          // Serial.println("-------------");
          Serial.println();
        }
      }
  }
  
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
  Serial.print("https://maps.google.com/maps?q=loc:"); printFloat(flat, 5); Serial.print(", "); printFloat(flon, 5);

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