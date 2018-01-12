#include <Servo.h> // library for servos
#include <SoftwareSerial.h> // TX RX software library for bluetooth
#include <EEPROM.h> // library for reading and writing to the EEPROM
#include <String.h> // library for manipulating strings

// constant definitions
#define SERVO1_PIN 9
#define SERVO2_PIN 10
#define SERVO3_PIN 10
#define BLUE_RX 6
#define BLUE_TX 5
#define ZERO 0
#define SZ 1024
#define KEY 123
#define PASS_LEN 20

// init bluetooth using pin constants for tx and rx
SoftwareSerial bluetooth(BLUE_TX, BLUE_RX);

// init servos
Servo servo_lock;
Servo servo_unlock;

int lservo;
int uservo;

// char array for reading values from EEPROM
char data[SZ];

void setup() {
  // attach servos
  servo_lock.attach(SERVO1_PIN);
  servo_unlock.attach(SERVO2_PIN);
  
  // Setup usb serial connection to computer with 9600 baud rate
  Serial.begin(9600);
  
  while (!Serial) 
  {
    ; // wait for serial port to connect.
  }

  //Setup Bluetooth serial connection to android with 9600 baud rate
  bluetooth.begin(9600);

  //reset servo positions to neutral positions
  servo_lock.write(0);
  delay(700);
  servo_unlock.write(90);
  delay(700);

  // Get size of EEPROM for debug
  Serial.print("Size of EEPROM: ");
  Serial.print(EEPROM.length(), DEC);
  Serial.print(" bytes.");
  Serial.println();

  // read all data from EEPROM
  for (int address = 0; address <= (SZ-1); ++address) { data[address] = EEPROM.read(address); }

  // check if first time loading
  if (data[SZ-1] == KEY) 
  {
    Serial.println("Device has already been setup!");
    return 0;
  }
  // it is first time setting up
  else 
  {
    char password[PASS_LEN];
    
    Serial.println("First time setup!");
    for (int addr = 0; addr <= (SZ-1); ++addr) 
    {
      EEPROM.write(addr, ZERO);
    }
    EEPROM.write(SZ-1, KEY);

    Serial.println("Please enter a password:");

    while (Serial.available())
    {
    Serial.readString().toCharArray(password, PASS_LEN);
    }
    
    for (int i = 0; i < sizeof(password); ++i)
    {
      Serial.print(password[i]);
    }
    Serial.println();
  }
}

void loop() {
  
  
  //Read from bluetooth and write pos (number) to servo
  if (bluetooth.available() > 0) // receive number from bluetooth (zero is no data / not available)
  {
    lservo = bluetooth.read(); // save the received number to servopos
    Serial.println(lservo); // serial print servopos current number received from bluetooth (use serial monitor)
    servo_lock.write(lservo); // rotate the servo the angle received from the android app
  }

}



