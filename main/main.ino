// Connor Noddin
// ECE 406
// Computer Engineering Capstone
#include <SPI.h>
#include <avr/pgmspace.h>
#include "ADNS9800.h"

// Packet Manipulation
#define R_BUTTON 0x02
#define L_BUTTON 0x01
#define M_BUTTON 0x04

// PS/2 Commands
#define RESET 0xff
#define BAT 0xAA
#define ID 0x00
#define ENABLE 0xF4
#define GET_DEV_ID 0xF2
#define STATUS_REQ 0xE9
#define ACK 0xFA

#define TIMEOUT 30

// Object for adns sensor
//controller adns_sensor;

// GPIO pin assignments for mouse buttons
const int LEFT = 2;  // Left mouse button
const int MIDDLE = 3;    // Middle mouse button
const int RIGHT = 4; // Right mouse button

// GPIO pin assignments for PS/2 connection
const int DATA_IN = 8;
const int CLK_IN = 7;
const int DATA_OUT = 6;
const int CLK_OUT = 5;

// GPIO pin assignments for sensor connection
const int SENSOR = 9;

adns::controller adns_sensor;

//Initial setup, run on boot
void setup() {

  int ret;

  //Initiate Serial communication for debugging
  Serial.begin(9600); //9600 bits/second (Baud rate)

  delay(500); // 500 ms delay for PS/2 standard

  // Initialize the mouse buttons as inputs:
  pinMode(LEFT, INPUT);
  pinMode(MIDDLE, INPUT);
  pinMode(RIGHT, INPUT);

  // In assignments for reading data and clock bus
  pinMode(DATA_IN, INPUT);
  pinMode(CLK_IN, INPUT);

  // Out assignments for controlling data and clock bus
  pinMode(DATA_OUT, OUTPUT);
  pinMode(CLK_OUT, OUTPUT);

  // Input for reading from sensor
  pinMode(SENSOR, INPUT);

  adns::controller adns_sensor();

  // Write self test passed
  ret = ps2_dwrite(BAT);

  // Write mouse ID
  ret = ps2_dwrite(ID);
}

// Run indefinitely on loop
void loop() {
  // Bit 3 is always 1 for byte 1
  byte byte_1 = 0x08, byte_2, byte_3; // 3 bytes for PS/2 packet

  byte tmp; //Temporary byte from functions

  uint16_t sensor_x, sensor_y; //Sensor x and y movement

  int ret;

  if ((digitalRead(DATA_IN) == LOW) || (digitalRead(CLK_IN) == LOW)) {
    while (ps2_dread(&tmp));
    ps2command(tmp);
  }

  tmp = get_button_states(); // Gets state of all three buttons

  byte_1 = byte_1 | tmp; //Saves states to byte 1

  // Code for sensor
  adns_sensor.loop();

  sensor_x = adns_sensor.get_x();
  sensor_y = adns_sensor.get_y();

  //Debugging
  //Serial.print("Byte 1: 0x");
  //Serial.print(byte_1, HEX);
  //Serial.print("\n");

  // Writes data to PS2 data out
  ret = ps2_dwrite(byte_1);
  ret = ps2_dwrite(byte_2);
  ret = ps2_dwrite(byte_3);

  delay(50); // Delay measured in ms
}

/* 
 Sets the clock to 1, then back to 0 after a delay 
 The clock frequency is 10-16.7 kHz.  The time from the rising edge of a clock pulse to a Data transition must be at least 5 microseconds. 
 The time from a data transition to the falling edge of a clock pulse must be at least 5 microseconds and no greater than 25 microseconds.
 Data is read from device to host on FALLING edge
*/
int ps2_clock(void)
{
  delayMicroseconds(20);
  digitalWrite(CLK_OUT, HIGH); //This is inverted
  delayMicroseconds(40);
  digitalWrite(CLK_OUT, LOW); //This is also inverted
  delayMicroseconds(20);
	return 0;
}

/* Writes data to the DATA_OUT PS/2 line
Summary: Bus States
Data = high, Clock = high:  Idle state.
Data = high, Clock = low:  Communication Inhibited.
Data = low, Clock = high:  Host Request-to-Send
EVERY OUTPUT IS INVERTED
*/
int ps2_dwrite(byte ps2_Data)
{

  int p = parity(ps2_Data); //Gets parity before bit manipulation

  //ps2_Data = ~ps2_Data; //Inverts bits because of open collector

  delayMicroseconds(1000); //Delay between bytes

  if (digitalRead(CLK_IN) == LOW) {
    return -1;
  }

  if (digitalRead(DATA_IN) == LOW) {
    return -2;
  }

  // First bit is always 0
  // INVERTED
  digitalWrite(DATA_OUT, HIGH);
  ps2_clock();

  //Send entire byte, LSB first
  //INVERTED because open collector
  for (int i = 0; i < 8; i++) {
    if ((ps2_Data & 0x01) == 0x01) digitalWrite(DATA_OUT, LOW); //Writes high if least significant bit is 0
    else digitalWrite(DATA_OUT, HIGH); //Writes low if least significant bit is 0
    ps2_clock(); //Clocks current data
    ps2_Data = ps2_Data >> 1; //Get next bit
  }

  // Check parity
  /* The parity bit is set if there is an even number of 1's in the data bits and reset (0) if there is an odd number of 1's in the data bits */
  if (p == 1) {
    digitalWrite(DATA_OUT, HIGH); //Low if odd number of ones
    ps2_clock();
  } else {
    digitalWrite(DATA_OUT, LOW); // High if even number of ones
    ps2_clock();
  }

  // Stop bit is always 1
  digitalWrite(DATA_OUT, LOW); // Always high
  ps2_clock();

  delayMicroseconds(1000); //Delay between bytes

  return 0;
}

/* Reads data from the DATA_IN ps/2 line  */
int ps2_dread(byte *read_in)
{
  unsigned int data = 0x00;
  unsigned int bit = 0x01;

  unsigned char calculated_parity = 1;
  unsigned char received_parity = 0;

  // Only reads when CLK is pulled low
  // Timesouts if host has not sent for 30 ms
  unsigned long init = millis();
  while((digitalRead(DATA_IN) != LOW) || (digitalRead(CLK_IN) != HIGH)) {
    if((millis() - init) > 30) return -1;
  }

  while (bit < 0x0100) {
    if (digitalRead(DATA_IN) == HIGH)
      {
        data = data | bit;
        calculated_parity = calculated_parity ^ 1;
      } else {
        calculated_parity = calculated_parity ^ 0;
      }

    bit = bit << 1;

  }

  // parity bit
  if (digitalRead(DATA_IN) == HIGH)
    {
      received_parity = 1;
    }

  // stop bit
  ps2_clock();

  digitalWrite(DATA_OUT, HIGH);
  ps2_clock();
  digitalWrite(DATA_OUT, HIGH);

  *read_in = data & 0x00FF;

  if (received_parity == calculated_parity) {
    return 0;
  } else {
    return -2;
  }

  return 0;
}

// Check parity of byte
int parity(byte p_check) 
{ 
  byte ones = 0; //Total number of ones

  for (int i = 0; i < 8; i++) {
    if ((p_check & 0x01) == 0x01) {
      ones++; //Adds to parity if lowest bit is 1
    }
    p_check = p_check >> 1; //Gets next bit
  }
 
  return (ones & 0x01); //Checks if parity is odd
}

int ps2command(byte input){

  unsigned char val;

  //This implements enough mouse commands to get by, most of them are
  //just acked without really doing anything

  switch (input) {
  case 0xFF: //reset
    ack();
    //the while loop lets us wait for the host to be ready
    while (ps2_dwrite(0xAA)!=0);
    while (ps2_dwrite(0x00)!=0);
    break;
  case 0xFE: //resend
    ack();
    break;
  case 0xF6: //set defaults
    //enter stream mode
    ack();
    break;
  case 0xF5:  //disable data reporting
    //FM
    ack();
    break;
  case 0xF4: //enable data reporting
    //FM
    //enabled = 1;
    ack();
    break;
  case 0xF3: //set sample rate
    ack();
    ps2_dread(&val); // for now drop the new rate on the floor
    //      Serial.println(val,HEX);
    ack();
    break;
  case 0xF2: //get device id
    ack();
    ps2_dwrite(00);
    break;
  case 0xF0: //set remote mode
    ack();
    break;
  case 0xEE: //set wrap mode
    ack();
    break;
  case 0xEC: //reset wrap mode
    ack();
    break;
  case 0xEB: //read data
    ack();
    //write_packet();
    break;
  case 0xEA: //set stream mode
    ack();
    break;
  case 0xE9: //status request
    ack();
    //      send_status();
    break;
  case 0xE8: //set resolution
    ack();
    ps2_dread(&val);
    //    Serial.println(val,HEX);
    ack();
    break;
  case 0xE7: //set scaling 2:1
    ack();
    break;
  case 0xE6: //set scaling 1:1
    ack();
    break;
  }
}

//ack a host command
void ack() {
  while (ps2_dwrite(0xFA));
}


byte get_button_states(void)
{
  int leftState, middleState, rightState; // If button is pressed or not

  byte buttons = 0x00;

  // read the state of the pushbutton value:
  leftState = digitalRead(LEFT);
  middleState = digitalRead(MIDDLE);
  rightState = digitalRead(RIGHT);

  //Default state of switches is high
  // Check the value of the left mouse button
  if (leftState == LOW) {
    buttons = buttons | L_BUTTON;
  } else {
    buttons = buttons & ~L_BUTTON;
  }
    // Check the value of the right mouse button
  if (rightState == LOW) {
    buttons = buttons | R_BUTTON;
  } else {
    buttons = buttons & ~R_BUTTON;
  }
  // Check the value of the middle mouse button
  if (middleState == LOW) {
    buttons = buttons | M_BUTTON;

  } else {
    buttons = buttons & ~M_BUTTON;
  }

  return buttons;
}
