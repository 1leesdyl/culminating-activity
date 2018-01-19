// Author: Dylan Lees
// Description: Serial Interface for Arduino and locking box (controlling Servos)
// Notes: all strings that are being read from Serial have one extra length to store the terminating character '\0'

#include <Servo.h> // library for manipulating servos
#include <EEPROM.h> // library for reading and writing to the EEPROM

// constant & macro definitions
#define SERVO1_PIN 9
#define SERVO2_PIN 10
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

// global variables for loop logic
bool match = false;
bool disp_prompt = true;

// function for iterating on a char array to change all characters to lower case
void stolower(char string[]) { for (int i = 0; i < strlen(string); ++i) string[i] = tolower(string[i]); }

// function for clearing screen (after password entering, etc.)
void clr(byte lines, int wait) 
{ 
  if (wait >= 0) { delay(wait); }
  for (int i = 0; i <= lines; ++i) Serial.println(); // print a bunch of lines
}

// read from EEPROM in range to a buffer (char array)
void readto(char buf[], unsigned int range) { for (int addr = 0; addr < range; ++addr) buf[addr] = EEPROM.read(addr); }

// get a Y/N response from Serial connection with error checking
bool ser_yn(const char text[])
{
  char answer[2]; // one extra for terminating char
  Serial.println(text);
  while (Serial)
  {
    while (Serial.available()) { delay(1); } // wait for all input to come through 
    String sanswer = Serial.readStringUntil('\n'); // stick data into buffer
    delay(1);
    if (sanswer != 0) { 
      sanswer.toCharArray(answer, sizeof(answer)); // convert string to char array for easier manipulation
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

// function for printing a menu and getting a number of option selected with error checking
int ser_menu(const String options[], const unsigned int len)
{
  bool prompt = true;

  // logic for deciding whether to make the buffer one char longer in case we are given more than 9 options
  byte asize = 1;
  if (len > 9) { asize = 2; }
  char answer[asize + 1] = {0};

  while (Serial) // while true basically, just a little error check
  {
    if (prompt) // check exists so prompt will not be spammed in the serial
    {
        Serial.println("Please choose an option: ");
        for (int i = 0; i < len; ++i)
        {
          Serial.print(i + 1, DEC);
          Serial.println(". " + options[i]);
        }
        prompt = false;
    }
    // wait until all data received
    while (Serial.available()) { delay(1); }

    // store data in temporary buffer
    String sanswer = Serial.readStringUntil('\n');
    delay(1); // wait for the serial to catch up
    sanswer.toCharArray(answer, sizeof(answer));
    if (strlen(answer) <= 0) { continue; } // if no answer
    unsigned int ianswer = atoi(answer);
    if (ianswer != 0 && ianswer <= len)  // if answer is valid
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

// function for writing characters (password to EEPROM)
void write_pw(const char pw[])
{
  for (int i = 0; i < PASSLEN; ++i) // iterate on all password bytes
  {
    if (pw[i] > 0) 
    {
      EEPROM.write(i, pw[i]);
      Serial.print("Writing char: "); // debug
      Serial.println(pw[i]);
    }
    else { EEPROM.write(i, ZERO); } // blank out null chars for error checking
  }
}

// function for setting a password via Serial interface
void set_pw()
{
  char password[PASSLEN + 1];

  // blank password array so null data does not get written to EEPROM
  for (int i = 0; i < sizeof(password); ++i) { password[i] = 0; }
  
  bool answer = true;

  Serial.println("Please enter a password up to 20 characters long: ");

  while (Serial) // error check
  {
    // wait until all data received
    while (Serial.available() > 0) { delay(1); }

    // store data into temporary string buffer
    String spassword = Serial.readStringUntil('\n');
    spassword.toCharArray(password, sizeof(password));
    delay(1);
    if (strlen(password) > 0) // if valid password
    { 
      Serial.println(password); // print password back to user for confirmation
      answer = ser_yn("Is this the password you want? (Y/N): ");
      if (answer) // yes to save password 
      {
        Serial.println("Saving password...");
        // blank out any existing data in the EEPROM
        for (int addr = 0; addr < PASSLEN; ++addr) { EEPROM.write(addr, ZERO); }

        // write password to EEPROM
        write_pw(password);
        Serial.println();

        // clear screen for security reasons
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
        // loop again because no valid password entered
        Serial.println("Please enter a password up to 20 characters long: ");
        continue; 
      }
    }
  }
}

// standard setup one time run function
void setup() {
  // attach servos
  lock_servo.attach(SERVO1_PIN);
  unlock_servo.attach(SERVO2_PIN);
  
  // Setup usb serial connection to computer with 9600 baud rate
  Serial.begin(9600);
  
  while (!Serial) {} // wait for serial port to connect

  Serial.println("Please set the Serial Monitor to send Newlines ONLY (\\n).");

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

// standard function for main logic
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
        unlock_servo.write(90);
        delay(700);
        lock_servo.write(180);
        delay(700);
        clr(100, 500);
        break;
      case 2:
        Serial.println("Closing box...");
        unlock_servo.write(180);
        delay(700);
        lock_servo.write(90);
        delay(700);
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

      if (match == true) { Serial.println("Correct password entered!"); }
    }
  }
}
