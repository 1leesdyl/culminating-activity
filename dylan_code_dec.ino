#include <Servo.h>

#define SERVO1_PIN 9
#define SERVO2_PIN 10

Servo servo1;
int servo1_pos = 0;
Servo servo2;
int servo2_pos = 0; 

void setup() {
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
}

void loop() {  
  for(servo1_pos = 0; servo1_pos < 180; servo1_pos += 1)
  {
    for(servo2_pos = 180; servo2_pos > 0; servo2_pos -= 1)
    { 
      servo1.write(servo1_pos);
      servo2.write(servo2_pos);
      delay(4);
    }
  }
}

