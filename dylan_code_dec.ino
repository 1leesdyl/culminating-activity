#include <Servo.h>

#define SERVO1_PIN 9
#define SERVO2_PIN 10
#define SERVO3_PIN 10

Servo servo1;
int servo1_pos = 0;
Servo servo2;
int servo2_pos = 0;
Servo servo3;
int servo3_pos = 0; 

Servo myservos[] = {servo1, servo2, servo3};

void forward(Servo servo) {
  servo.write(0);
  delay(700);
  servo.write(180);
  delay(700);
}

void setup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO2_PIN);
}

void loop() {
  int i;
  for (i = 0; i < 3; i += 1) {
    forward(myservos[i]);
  }  
}

