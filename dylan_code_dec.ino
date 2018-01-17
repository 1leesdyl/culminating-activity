#include <Servo.h> // library for servos
#include <EEPROM.h> // library for reading and writing to the EEPROM
#include <String.h> // library for manipulating strings

// constant definitions
#define SERVO1_PIN 9
#define SERVO2_PIN 10
#define SERVO3_PIN 10
#define ZERO 0
#define KEY 123
#define PASSLEN 20

const int SZ = EEPROM.length();

// declare servos
Servo lock_servo;
Servo unlock_servo;

// declare pw array
char pw[PASSLEN + 1];

// declare working options
String woption[3] = {"Open box", "Close box", "Change password"};

bool debug = false;

void ser_flush() { while(Serial.available() > 0) Serial.read(); }

void stolower(char string[]) { for (int i = 0; i < strlen(string); ++i) string[i] = tolower(string[i]); }

void readto(char buf[], unsigned int range) { for (int addr = 0; addr < range; ++addr) buf[addr] = EEPROM.read(addr); }

bool ser_yn(const char text[])
{
  char answer[2];
  Serial.println(text);
  do {
    delay(1);
    if (Serial.available() > 0) 
    { 
      Serial.readStringUntil('\n').toCharArray(answer, sizeof(answer));
      stolower(answer);
    }
  } while (answer[0] != 'y' || answer[0] != 'n');
  
  return (answer[0] == 'y');
}

int ser_menu(const String options[])
{
  Serial.println("Please choose an option: ");
  for (int i = 0; i < sizeof(options); ++i)
  {
    Serial.print(i + 1, DEC);
    Serial.print(". " + options[i]);
  }
  byte asize = 1;
  if (sizeof(options) > 9) { asize = 2; }
  char answer[asize + 1] = {0};
  
  do 
  {
    delay(1);
    if (Serial.available() > 0) { Serial.readStringUntil('\n').toCharArray(answer, sizeof(answer)); }
  } while(answer == 0 || answer > sizeof(options));
}

void write_pw(const char pw[])
{
  for (int addr = 0; addr < PASSLEN; ++addr)
  {
    EEPROM.write(addr, pw[addr]);
    Serial.print("Writing char: ");
    Serial.print(pw[addr]);
    Serial.println();
  }
}

void set_pw()
{
  char password[PASSLEN + 1];
  bool answer = false;

  Serial.println("Please enter a password up to 20 char:"); // todo use pass len
  
  do {
    delay(1);
    if (Serial.available() > 0) 
    { 
      Serial.readStringUntil('\n').toCharArray(password, sizeof(password));
      Serial.println(password);
      answer = ser_yn("Is this the password you want? (Y/N): ");
      if (answer) 
      {
        Serial.println("Saving password...");
        write_pw(password);
        Serial.println();
        if (ser_yn("Would you like to list the bytes of the EEPROM? (Y/N): "))
        {
          for (int addr = 0; addr < SZ; ++addr)
          {    
            Serial.print(addr);
            Serial.print("\t");
            Serial.print(EEPROM.read(addr), DEC);
            Serial.println();
          }
        }
        else { break; }
      } 
      else { continue; }
    }
    else { continue; }
  } while (answer = false);
}

void setup() {
  // attach servos
  lock_servo.attach(SERVO1_PIN);
  unlock_servo.attach(SERVO2_PIN);
  
  // Setup usb serial connection to computer with 9600 baud rate
  Serial.begin(9600);
  
  while (!Serial) { ; } // wait for serial port to connect

  Serial.println("Please set the Serial Monitor to send Newlines and Carriage Returns (NL and CR).");

  // reset servo positions to neutral positions
  lock_servo.write(0);
  delay(700);
  unlock_servo.write(90);
  delay(700);

  // Get size of EEPROM for debug
  Serial.print("Size of EEPROM: ");
  Serial.print(SZ, DEC);
  Serial.print(" bytes.");
  Serial.println();
  
  byte key = EEPROM.read(SZ-1);

  // check if first time loading
  if (key == KEY) 
  {
    Serial.println("Device has already been setup!");
    return 0;
  }
  
  // it is first time setting up
  else 
  {
    char password[PASSLEN + 1];
    
    Serial.println("First time setup!");
    for (int addr = 0; addr < SZ; ++addr) { EEPROM.write(addr, ZERO); }
    
    //EEPROM.write(SZ-1, KEY);

    set_pw();
  }
  
  readto(pw, PASSLEN);
  //Serial.println("Now in loop...");
}

void loop() {
  char input[PASSLEN + 1];
  bool match = false;

  Serial.println("Please enter a password: ");
  
  do {
    delay(1);
    if (Serial.available() > 0) { Serial.readStringUntil('\n').toCharArray(input, sizeof(input)); }
    match = true; // assume match
    for (int i = 0; i < PASSLEN; ++i) 
    { 
      if (input[i] != EEPROM.read(i)) 
      { 
        match = false;
        Serial.println("Wrong password!");
        break;
      }
    }
  } while (match == false);
  
  if (match == true)
  {
    Serial.println("Correct password entered!");
    unsigned int sel = ser_menu(woption);
    switch (sel) {
      case 1:
        Serial.println("Opening box...");
        unlock_servo.write(0);
        lock_servo.write(0);
        break;
      case 2:
        Serial.println("Closing box...");
        unlock_servo.write(0);
        lock_servo.write(0);
        break;
      case 3:
        set_pw();
        break;
      default:
        Serial.println("Incorrect option!");
        break;
    }
  }
}

 
