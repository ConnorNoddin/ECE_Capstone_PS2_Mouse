// Connor Noddin
// ECE 406
// Computer Engineering Capstone
// ps2mouse.cpp

#include "ps2mouse.h"
#include <avr/pgmspace.h>
#include <Arduino.h>
#include "ADNS3050.h"


/* 
 Description: Sets the clock to 1, then back to 0 after a delay 
 The clock frequency is 10-16.7 kHz. 
 Data is read from device to host on FALLING edge
 Data is read from host to device on RISING edge
*/
int ps2_clock(void) {
  delayMicroseconds(CLOCK_HALF);
  digitalWrite(CLK_OUT, HIGH);  //This is inverted
  delayMicroseconds(CLOCK_FULL);
  digitalWrite(CLK_OUT, LOW);  //This is also inverted
  delayMicroseconds(CLOCK_HALF);
  return 0;
}

/* 
Description: Sends 11 bit packet to PS2 data line if communication
is not being inhibited
Summary: Bus States
Data = high, Clock = high:  Idle state.
Data = high, Clock = low:  Communication Inhibited.
Data = low, Clock = high:  Host Request-to-Send
*/
int ps2_dwrite(byte ps2_Data) {

  int p = parity(ps2_Data);  //Gets parity before bit manipulation

  delayMicroseconds(BYTE_DELAY);  //Delay between bytes

  // Never transmit if the host is inhibiting communication
  if (digitalRead(CLK_IN) == LOW) {
    return -1;
  } else if (digitalRead(DATA_IN) == LOW) {
    return -2;
  }

  // First bit is always 0
  //Logic is inverted because open collector
  digitalWrite(DATA_OUT, HIGH);
  ps2_clock();

  //Send entire byte, LSB first
  //Logic is inverted because open collector
  for (int i = 0; i < 8; i++) {
    if ((ps2_Data & 0x01) == 0x01) digitalWrite(DATA_OUT, LOW);  //Writes high if least significant bit is 0
    else digitalWrite(DATA_OUT, HIGH);                           //Writes low if least significant bit is 0
    ps2_clock();                                                 //Clocks current data
    ps2_Data = ps2_Data >> 1;                                    //Get next bit
  }

  // The parity bit is set if there is an even number of 1's
  // in the data bits and reset (0) if there is an odd number of 1's in the data bits
  // The parity bit forces odd parity in the packet
  if (p == 1) {
    digitalWrite(DATA_OUT, HIGH);  //Low if odd number of ones
    ps2_clock();
  } else {
    digitalWrite(DATA_OUT, LOW);  // High if even number of ones
    ps2_clock();
  }

  // Stop bit is always 1
  digitalWrite(DATA_OUT, LOW);  // Always high
  ps2_clock();

  delayMicroseconds(BYTE_DELAY);  //Delay between bytes

  return 0;
}

/* 
Description: Reads 11 bit data packet from PS2 data line.
Also sends ACK bit, for a total 12 bit packet. This also
detects if the host timeouts
*/
int ps2_dread(byte* read_in) {
  unsigned int data = 0x00;
  unsigned int bit = 0x01;

  unsigned char calculated_parity = 1;
  unsigned char received_parity = 0;

  // Only reads when CLK is pulled low
  // Timesouts if host has not sent for 30 ms
  // This is effectively the start bit
  unsigned long init = millis();
  while (((digitalRead(DATA_IN) != LOW) || (digitalRead(CLK_IN) != HIGH))) {
    if ((millis() - init) > HOST_TIMEOUT) {
      Serial.println("Read failed, host timeout!");
      return -1;
    }
  }

  //First packet bit is here which is always 0!
  ps2_clock();

  // Reads 8 data bits with LSB first
  for (int i = 0; i < 8; i++) {

    if (digitalRead(DATA_IN) == HIGH) {
      data = data | bit;
      calculated_parity = calculated_parity ^ 1;
    }

    bit = bit << 1;
    ps2_clock();
  }

  // parity bit ... clock is from last iteration of loop
  if (digitalRead(DATA_IN) == HIGH) {
    received_parity = 1;
  }

  //Clock for stop bit
  ps2_clock();

  // Acks the packet that was just read
  digitalWrite(DATA_OUT, HIGH);
  delayMicroseconds(CLOCK_HALF);
  digitalWrite(CLK_OUT, HIGH);
  delayMicroseconds(CLOCK_FULL);
  digitalWrite(CLK_OUT, LOW);
  delayMicroseconds(CLOCK_HALF);
  digitalWrite(DATA_OUT, LOW);

  // Returns data to main loop
  *read_in = data & 0x00FF;

  //Parity check
  if (received_parity == calculated_parity) {
    return 0;
  } else {
    //Parity is wrong and an error occured
    Serial.print("Calculated Parity: ");
    Serial.print(calculated_parity, DEC);
    Serial.print("\t Received Parity: ");
    Serial.println(received_parity, DEC);
    Serial.println("Parity error in read function!");
    return -2;
  }

  return 0;
}

/*
Description: Checks parity of byte. This is odd parity. Consequently, if the parity
of the byte is odd, this returns a one
*/
int parity(byte p_check) {
  byte ones = 0;  //Total number of ones

  for (int i = 0; i < 8; i++) {
    if ((p_check & 0x01) == 0x01) {
      ones++;  //Adds to parity if lowest bit is 1
    }
    p_check = p_check >> 1;  //Gets next bit
  }

  return (ones & 0x01);  //Checks if parity is odd
}

/*
Description: Takes a byte that was read from the host as an input
Then, based off the byte, sends the exact appropriate response.
This function includes every possible command the host can send to the device.
If the command is not in this list, it is not a legal command.
*/
int ps2_command(byte input) {

  unsigned char val;  //Value for when host is sending data for a command

  switch (input) {
    case RES:  //Resets mouse
      ack();
      while (ps2_dwrite(BAT) != 0);
      while (ps2_dwrite(ID) != 0);
      break;
    case RESEND:  //Error occured
      ack();
      break;
    case DEF:  //Sets defaults
      ack();
      break;
    case DISABLE:  //Disables data reporting
      ack();
      break;
    case ENABLE:  //Enables data reporting ...0xF4
      Serial.println("Enable signal received!");
      ack();
      break;
    case SET_RATE:  //Reads sample rate
      ack();
      ps2_dread(&val);
      ack();
      break;
    case GET_DEV_ID:  //Gets device id
      ack();
      ps2_dwrite(ID);  //0x03 also works
      break;
    case REMOTE:  //Sets remote mode
      ack();
      break;
    case WRAP:  //Sets wrap mode
      ack();
      break;
    case RES_WRAP:  //Resets wrap mode
      ack();
      break;
    case READ:  //Reads data
      ack();
      break;
    case STREAM:  //Enables stream mode
      ack();
      break;
    case STATUS_REQ:  //Requests status of mouse
      ack();
      while (ps2_dwrite(ID) != 0);
      while (ps2_dwrite(STAT_1) != 0);
      while (ps2_dwrite(STAT_2) != 0);
      break;
    case SET_RES:  //Sets resolution of sensor
      ack();
      ps2_dread(&val);
      ack();
      break;
    case SCALE_2:  //Sets scaling 2:1
      ack();
      break;
    case SCALE_1:  //Sets scaling 1:1
      ack();
      break;
    default:
      Serial.print("Unknown command.... likely an error in the read function");
      break;
  }
  Serial.print("Sent response to command: 0x");
  Serial.println(input, HEX);
  return 0;
}

/*
Description: Acks a host command. Per the PS2 standard,
this must complete during the handshake. Consequently, this keeps
attempts to do so until successfull.
*/
void ack() {
  while (ps2_dwrite(ACK) != 0);  //0xFA
}

/*
Description: Returns the states of the buttons as the least
significant bits of a single byte. Works for a button being
off as well as on
*/
byte get_button_states(void) {
  int leftState, middleState, rightState;  // If button is pressed or not

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

/*
Description: Returns 3 bytes. First is control, second is X movement, third
is Y movement
*/
byte* get_bytes(void) {

  byte tmp;                                  //Temporary byte from functions
  int sensor_x, sensor_y;                    //Sensor x and y movement
  static byte data[3];                       //3 data packets
  byte byte_1 = BYTE_1_BIT, byte_2, byte_3;  // 3 bytes for PS/2 packet

  tmp = get_button_states();  // Gets state of all three buttons

  byte_1 = byte_1 | tmp;  //Saves states to byte 1

  // Code for sensor
  sensor_x = getX();  //Extract change in x
  sensor_y = getY();  //Extract change in y

  //Sets sign bits from sensor
  if (sensor_x < 0)
    byte_1 = byte_1 | X_SIGN;
  else
    byte_1 = byte_1 & ~X_SIGN;

  if (sensor_y < 0)
    byte_1 = byte_1 | Y_SIGN;
  else
    byte_1 = byte_1 & ~Y_SIGN;

  //Sets overflow bits from sensor in 2s compliment
  if (sensor_x > 255) {
    byte_1 = byte_1 | X_OVERFLOW;
    sensor_x = 0xFF;
  } else {
    byte_1 = byte_1 & ~X_OVERFLOW;
  }
  if (sensor_x < -255) {
    byte_1 = byte_1 | X_OVERFLOW;
    sensor_x = 0x01;  //2s compliment, sign bit above
  } else {
    byte_1 = byte_1 & ~X_OVERFLOW;
  }

  if (sensor_y > 255) {
    byte_1 = byte_1 | Y_OVERFLOW;
    sensor_y = 0xFF;
  } else {
    byte_1 = byte_1 & ~Y_OVERFLOW;
  }
  if (sensor_y < -255) {
    byte_1 = byte_1 | Y_OVERFLOW;
    sensor_x = 0x01;  //2s compliment, sign bit above
  } else {
    byte_1 = byte_1 & ~Y_OVERFLOW;
  }

  //Gets lower 8 bits of both sensor data for movement
  byte_2 = sensor_x & 0x00FF;
  byte_3 = sensor_y & 0x00FF;

  data[0] = byte_1;
  data[1] = byte_2;
  data[2] = byte_3;

  return data;
}
