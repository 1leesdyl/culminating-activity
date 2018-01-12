#include <EEPROM.h>

#define SZ 1024
#define ZERO 0
#define KEY 123

void setup() {
  // run once:
  // Setup usb serial connection to computer with 9600 baud rate
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  
  for (int addr = 0; addr <= (SZ-1); ++addr) {
    EEPROM.write(addr, ZERO);
  }

  //EEPROM.write(1023, KEY);
  
  for (int address = 0; address <= (SZ-1); ++address)
  {    
    Serial.print(address);
    Serial.print("\t");
    Serial.print(EEPROM.read(address), DEC);
    Serial.println();
  }
}

void loop() { /* not needed */ }
