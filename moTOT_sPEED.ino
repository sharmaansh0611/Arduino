
int ENA = 9;
int speed;
void setup() {
  // put your setup code here, to run once:

  pinMode(ENA, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
    
    
    for (speed = 200; speed>=60; speed--) {
      analogWrite(ENA, speed);
      delay(100);
    }
    

    delay(3000);
    
    for (speed =60; speed<=200; speed++) {
      analogWrite(ENA, speed);
      delay(100);
    }
   
    delay(3000);
}
