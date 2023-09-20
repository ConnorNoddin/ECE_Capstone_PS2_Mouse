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
const int left = 2;  // Left mouse button
const int middle = 3;    // Middle mouse button
const int right = 4; // Right mouse button

// GPIO pin assignments for PS/2 connection
const int DATA = 5;
const int CLK = 6;

int leftState, middleState, rightState; // If button is pressed or not

byte byte_1, byte_2, byte_3; // 3 bytes for PS/2 packet

//Initial setup, run on boot
void setup() {

  //Initiate Serial communication for debugging
  Serial.begin(9600); //9600 bits/second (Baud rate)

  delay(500); // 500 ms delay for PS/2 standard

  // Initialize the mouse buttons as inputs:
  pinMode(left, INPUT);
  pinMode(middle, INPUT);
  pinMode(right, INPUT);

  pinMode(DATA, OUTPUT);
  pinMode(CLK, OUTPUT);

  byte_1 = byte_1 | 0x08; // Bit 3 is always 1

}

// Run indefinitely on loop
void loop() {

  // read the state of the pushbutton value:
  leftState = digitalRead(left);
  middleState = digitalRead(middle);
  rightState = digitalRead(right);

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

  delay(10); // Delay measured in ms
}

/* Sets the clock to 1, then back to 0 after a delay 
 The clock frequency is 10-16.7 kHz.  The time from the rising edge of a clock pulse to a Data transition must be at least 5 microseconds. 
 The time from a data transition to the falling edge of a clock pulse must be at least 5 microseconds and no greater than 25 microseconds. 
*/
int ps2_clock(void)
{
  //Sets clock to high
	digitalWrite(CLK, HIGH);
  delayMicroseconds(15);
	digitalWrite(CLK, LOW);
  //delayMicroseconds(15);
	return 0;
}

/* Writes data to the DATA PS/2 line */
int ps2_dwrite(byte ps2_Data)
{

  int p; // Parity check

  // First bit is always 0
  digitalWrite(DATA, LOW);
  ps2_clock();

  p = parity(ps2_Data); //Gets parity before bit shift

  //Send entire byte, LSB first
  for (int i = 0; i < 8; i++) {
    if ((ps2_Data & 0x01) == 0x01) digitalWrite(DATA, HIGH); //Writes high if least significant bit is 0
    else digitalWrite(DATA, LOW); //Writes low if least significant bit is 0
    ps2_clock(); //Clocks current data
    ps2_Data = ps2_Data >> 1; //Get next bit
  }

  // Check parity
  if (parity == 1) {
    digitalWrite(DATA, LOW); //Low if odd number of ones
    ps2_clock();
  } else {
    digitalWrite(DATA, HIGH); // High if even number of ones
    ps2_clock();
  }

  // Stop bit is always 1
  digitalWrite(DATA, HIGH); // Always high
  ps2_clock();
}

// Check parity of byte
int parity(byte p_check) 
{ 
  byte ones = 0; //Total number of ones

  for (int i = 0; i < 8; i++) {
    if ((p_check & 0x01) == 0x01) {
      ones++;
    }
    p_check = p_check >> 1;
}
 

  return (ones & 0x01); //Checks if parity is odd
}

