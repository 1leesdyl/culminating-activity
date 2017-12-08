#include <SoftwareSerial.h>
#include <Servo.h>

#define SERVO1_PIN 9
#define SERVO2_PIN 10
#define RX 3
#define TX 2

SoftwareSerial mySerial(TX, RX);
String fromAndroid = "";

int pwm3 = 3;

Servo servo1;
int servo1_pos = 0;
Servo servo2;
int servo2_pos = 0; 

boolean stringComplete = false;  

// Move servo along angle
void servo_angle(Servo myservo, int mypos, int angle, int speed = 1) {
  for (mypos = 0; mypos <= angle; mypos += speed) { 
    myservo.write(mypos);              
    delay(15);                       
  }
}

// Move servo along angle and return
/*
void sweep_angle(Servo myservo, int mypos, int v = 1, int angle = 180) {
  servo_angle
}
*/


void setup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
}

void loop() {  
  servo_angle(servo1, servo1_pos, 180);
  servo_angle(servo1, servo1_pos, -180, -1);
}

