// Connor Noddin
// ECE 406
// Computer Engineering Capstone

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

// GPIO pin assignments for mouse buttons
const int LEFT = 2;  // Left mouse button
const int MIDDLE = 3;    // Middle mouse button
const int RIGHT = 4; // Right mouse button

// GPIO pin assignments for PS/2 connection
const int DATA_IN = 5;
const int CLK_IN = 6;
const int DATA_OUT = 7;
const int CLK_OUT = 8;

// GPIO pin assignments for sensor connection
const int SENSOR = 9;

//Initial setup, run on boot
void setup() {

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
  pinMode(SENSOR, OUTPUT);
}

// Run indefinitely on loop
void loop() {

  int leftState, middleState, rightState; // If button is pressed or not

  // Bit 3 is always 1 for byte 1
  byte byte_1 = 0x08, byte_2, byte_3; // 3 bytes for PS/2 packet

  int sensor_x, sensor_y;

  // read the state of the pushbutton value:
  leftState = digitalRead(LEFT);
  middleState = digitalRead(MIDDLE);
  rightState = digitalRead(RIGHT);

  //Default state of switches is high
  // Check the value of the left mouse button
  if (leftState == LOW) {
    byte_1 = byte_1 | L_BUTTON;
  } else {
    byte_1 = byte_1 & ~L_BUTTON;
  }
    // Check the value of the right mouse button
  if (rightState == LOW) {
    byte_1 = byte_1 | R_BUTTON;
  } else {
    byte_1 = byte_1 & ~R_BUTTON;
  }
  // Check the value of the middle mouse button
  if (middleState == LOW) {
    byte_1 = byte_1 | M_BUTTON;

  } else {
    byte_1 = byte_1 & ~M_BUTTON;
  }

  //Debugging
  Serial.print("Byte 1: 0x");
  Serial.print(byte_1, HEX);
  Serial.print("\n");

  // Writes data to PS2 data out
  ps2_dwrite(byte_1);
  ps2_dwrite(byte_2);
  ps2_dwrite(byte_3);

  delay(10); // Delay measured in ms
}

/* 
 Sets the clock to 1, then back to 0 after a delay 
 The clock frequency is 10-16.7 kHz.  The time from the rising edge of a clock pulse to a Data transition must be at least 5 microseconds. 
 The time from a data transition to the falling edge of a clock pulse must be at least 5 microseconds and no greater than 25 microseconds. 
*/
int ps2_clock(void)
{
  //Sets clock to high
	digitalWrite(CLK_OUT, HIGH);
  delayMicroseconds(15);
	digitalWrite(CLK_OUT, LOW);
  //delayMicroseconds(15);
	return 0;
}

/* Writes data to the DATA_OUT PS/2 line */
int ps2_dwrite(byte ps2_Data)
{

  int p = parity(ps2_Data); //Gets parity before bit shift

  // First bit is always 0
  digitalWrite(DATA_OUT, LOW);
  ps2_clock();

  //Send entire byte, LSB first
  for (int i = 0; i < 8; i++) {
    if ((ps2_Data & 0x01) == 0x01) digitalWrite(DATA_OUT, HIGH); //Writes high if least significant bit is 0
    else digitalWrite(DATA_OUT, LOW); //Writes low if least significant bit is 0
    ps2_clock(); //Clocks current data
    ps2_Data = ps2_Data >> 1; //Get next bit
  }

  // Check parity
  if (p == 1) {
    digitalWrite(DATA_OUT, LOW); //Low if odd number of ones
    ps2_clock();
  } else {
    digitalWrite(DATA_OUT, HIGH); // High if even number of ones
    ps2_clock();
  }

  // Stop bit is always 1
  digitalWrite(DATA_OUT, HIGH); // Always high
  ps2_clock();

  return 0;
}

/* Reads data from the DATA_IN ps/2 line  */
int ps2_dread(byte ps2_Data)
{

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

