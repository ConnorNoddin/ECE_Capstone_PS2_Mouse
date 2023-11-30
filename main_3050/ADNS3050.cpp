// Connor Noddin
// ECE 406
// Computer Engineering Capstone
// ADNS3050.cpp

#include <SPI.h>
#include "ADNS3050.h"

/* 
 Description: Beings communications using
 SS/NCS pin
*/
void com_start() {
  digitalWrite(PIN_NCS, HIGH);
  delay(20);
  digitalWrite(PIN_NCS, LOW);
}

/* 
 Description: Reads a register from the ADNS3050 sensor
*/
byte Read(byte reg_addr) {
  digitalWrite(PIN_NCS, LOW);  //begin communication
  // send address of the register, with MSBit = 0 to say it's reading
  SPI.transfer(reg_addr & 0x7f);
  delayMicroseconds(100);
  // read data
  byte data = SPI.transfer(0);
  delayMicroseconds(30);
  digitalWrite(PIN_NCS, HIGH);  //end communication
  delayMicroseconds(30);

  return data;
}

/* 
 Description: Writes to a register on the ADNS3050
*/
void Write(byte reg_addr, byte data) {
  digitalWrite(PIN_NCS, LOW);
  //send address of the register, with MSBit = 1 to say it's writing
  SPI.transfer(reg_addr | 0x80);
  //send data
  SPI.transfer(data);
  delayMicroseconds(30);
  digitalWrite(PIN_NCS, HIGH);  //end communication
  delayMicroseconds(30);
}

/* 
 Description: Starup protocol. Must be run to initialize sensor
*/
void startup() {
  //--------Setup SPI Communication---------
  /*
  byte out = 0;
  byte read = 0;
  byte bit = 0;
  */
  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_NCS, OUTPUT);
  SPI.begin();
  // set the details of the communication
  SPI.setBitOrder(MSBFIRST);             // transimission order of bits
  SPI.setDataMode(SPI_MODE3);            // sampling on rising edge
  SPI.setClockDivider(SPI_CLOCK_DIV16);  // 16MHz/16 = 1MHz
  delay(10);

  //----------------- Power Up and config ---------------
  com_start();
  Write(RESET, 0x5a);        // force reset
  delay(100);                // wait for it to reboot
  Write(MOUSE_CTRL, 0x20);   //Setup Mouse Control
  Write(MOTION_CTRL, 0x00);  //Clear Motion Control register
  delay(100);
  Write(MISC_SETTINGS, B0000001); // Liftoff detection disable
  delay(100);
  Write(MOUSE_CTRL,B11000); // 2000 DPI
  delay(100);
}

/* 
 Description: Convert register value from 2s complement to a regular
 32 bit integer
*/
int convTwosComp(int b) {  //Convert from 2's complement
  if (b & 0x80) {
    b = -1 * ((b ^ 0xff) + 1);
  }
  return b;
}

/* 
 Description: Reads X movement from sensor register
*/
int getX() {  //returns the X acceleration value
  byte x = 0;
  x = Read(0x03);
  return (convTwosComp(x));
}

/* 
 Description: Reads Y movement from sensor register
*/
int getY() {  //returns the Y acceleration value
  byte y = 0;
  y = Read(0x04);
  return (convTwosComp(y));
}
