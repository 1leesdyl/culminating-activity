// Author: Dylan Lees
// Description: Serial Interface for Arduino and locking box (controlling Servos)

#include <Servo.h> // library for servos
#include <EEPROM.h> // library for reading and writing to the EEPROM

// constant & macro definitions
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
String woption[] = {"Open box", "Close box", "Change password", "Exit"};

bool match = false;
bool disp_prompt = true;

void stolower(char string[]) { for (int i = 0; i < strlen(string); ++i) string[i] = tolower(string[i]); }

void clr(byte lines, int wait) 
{ 
  if (wait >= 0) { delay(wait); }
  for (int i = 0; i <= lines; ++i) Serial.println(); 
}

void readto(char buf[], unsigned int range) { for (int addr = 0; addr < range; ++addr) buf[addr] = EEPROM.read(addr); }

bool ser_yn(const char text[])
{
  char answer[2];
  Serial.println(text);
  while (Serial)
  {
    while (Serial.available()) { delay(1); }
    String sanswer = Serial.readStringUntil('\n');
    delay(1);
    if (sanswer != 0) { 
      sanswer.toCharArray(answer, sizeof(answer));
      stolower(answer);
      Serial.println(answer);
      switch (answer[0])  
      {
        case 'y':
          return true;
          break;
        case 'n':
          return false;
          break;
        default:
          Serial.println(text);
          continue;
      }
    }
  }
}

int ser_menu(const String options[], const unsigned int len)
{
  bool prompt = true;
  
  byte asize = 1;
  if (len > 9) { asize = 2; }
  char answer[asize + 1] = {0};

  while (Serial)
  {
    if (prompt)
    {
        Serial.println("Please choose an option: ");
        for (int i = 0; i < len; ++i)
        {
          Serial.print(i + 1, DEC);
          Serial.println(". " + options[i]);
        }
        prompt = false;
    }
    while (Serial.available()) { delay(1); }
    String sanswer = Serial.readStringUntil('\n');
    delay(1);
    sanswer.toCharArray(answer, sizeof(answer));
    if (strlen(answer) <= 0) { continue; }
    unsigned int ianswer = atoi(answer);
    if (ianswer != 0 && ianswer <= len) 
    { 
      return ianswer;
    }
    else
    {
      Serial.println("Bad Option!");
      prompt = true;
    }
  }
}

void write_pw(const char pw[])
{
  for (int i = 0; i < PASSLEN; ++i)
  {
    if (pw[i] > 0) 
    {
      EEPROM.write(i, pw[i]);
      Serial.print("Writing char: ");
      Serial.println(pw[i]);
    }
    else { EEPROM.write(i, ZERO); }
  }
}

void set_pw()
{
  char password[PASSLEN + 1];

  for (int i = 0; i < sizeof(password); ++i) { password[i] = 0; }
  
  bool answer = true;

  Serial.println("Please enter a password up to 20 characters long: ");

  while (Serial)
  {
    while (Serial.available() > 0) { delay(1); }
    String spassword = Serial.readStringUntil('\n');
    spassword.toCharArray(password, sizeof(password));
    delay(1);
    if (strlen(password) > 0) 
    { 
      Serial.println(password);
      answer = ser_yn("Is this the password you want? (Y/N): ");
      if (answer) 
      {
        Serial.println("Saving password...");
        for (int addr = 0; addr < PASSLEN; ++addr) { EEPROM.write(addr, ZERO); }
        write_pw(password);
        Serial.println();
        clr(100, 2000);
        if (ser_yn("Would you like to list the bytes of the EEPROM? (Y/N): "))
        {
          for (int addr = 0; addr < PASSLEN; ++addr)
          {    
            Serial.print(addr);
            Serial.print("\t");
            Serial.print(EEPROM.read(addr), DEC);
            Serial.println();
          }

          break;
        }
        else { break; }
      }
      else 
      { 
        Serial.println("Please enter a password up to 20 characters long: ");
        continue; 
      }
    }
  }
}

void setup() {
  // attach servos
  lock_servo.attach(SERVO1_PIN);
  unlock_servo.attach(SERVO2_PIN);
  
  // Setup usb serial connection to computer with 9600 baud rate
  Serial.begin(9600);
  
  while (!Serial) {} // wait for serial port to connect

  Serial.println("Please set the Serial Monitor to send Newlines (\\n).");

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

    // reset password bytes
    for (int addr = 0; addr < PASSLEN; ++addr) { EEPROM.write(addr, ZERO); }

    set_pw();
    EEPROM.write(SZ-1, KEY);
  }
  
  readto(pw, PASSLEN);
  //Serial.println("Now in loop...");
}

void loop() {
  char input[PASSLEN + 1];
  for (int i = 0; i < sizeof(input); ++i) { input[i] = 0; }

  if (disp_prompt) 
  {
    Serial.println("Please enter the password: ");
    disp_prompt = false;
  }

  if (match == true)
  {    
    unsigned int sel = ser_menu(woption, (sizeof(woption) / sizeof(woption[0])));
    switch (sel) {
      case 1:
        Serial.println("Opening box...");
        unlock_servo.write(0);
        lock_servo.write(0);
        clr(100, 500);
        break;
      case 2:
        Serial.println("Closing box...");
        unlock_servo.write(0);
        lock_servo.write(0);
        clr(100, 500);
        break;
      case 3:
        set_pw();
        match = false;
        disp_prompt = true;
        break;
      case 4:
        Serial.println("Goodbye!");
        delay(3000);
        exit(0); 
      default:
        Serial.println("Incorrect option!");
        break;
    }
  }
  else
  {
    for (int i = 0; i < sizeof(input); ++i) { input[i] = 0; }
    while (Serial.available()) { delay(1); }
    String sinput = Serial.readStringUntil('\n');
    delay(1);
    if (sinput != 0) 
    { 
      sinput.toCharArray(input, sizeof(input));

      char password[PASSLEN + 1];
      readto(password, PASSLEN);

      match = true;
      
      for (int i = 0; i < PASSLEN; ++i)
      { 
        if (input[i] != password[i]) 
        {
          match = false;
          disp_prompt = true;
          Serial.println("Bad Password!");
          break;
        }
      }

      if (match == true) 
      {
        Serial.println("Correct password entered!");
      }
    }
  }
}
