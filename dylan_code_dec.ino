#include <Servo.h>

#define SERVO1_PIN 9
#define SERVO2_PIN 10

Servo servo1;
int servo1_pos = 0;
Servo servo2;
int servo2_pos = 0; 

boolean stringComplete = false;  

// Move servo along angle
void servo_angle(Servo myservo, int mypos, int angle, int inc) {
  angle -= mypos;
  for (int pos = mypos; pos <= angle; mypos += inc) { 
    myservo.write(mypos);              
    delay(15);                       
  }
}

void setup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
}
p
void loop() {  
  servo_angle(servo1, servo1_pos, 180, 5);
}

