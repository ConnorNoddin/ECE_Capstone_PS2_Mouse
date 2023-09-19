// Connor Noddin
// ECE 406
// Computer Engineering Capstone

#define R_BUTTON 0x02
#define L_BUTTON 0x01
#define M_BUTTON 0x04

#define RESET 0xff

// GPIO pin assignments for mouse buttons
const int left = 5;  // Left mouse button
const int middle = 6;    // Middle mouse button
const int right = 7; // Right mouse button

int leftState, middleState, rightState; // If button is pressed or not

byte byte_1, byte_2, byte_3;

//Initial setup, run on boot
void setup() {

  //Initiate Serial communication for debugging
  Serial.begin(9600); //9600 bits/second (Baud rate)

  // Initialize the mouse buttons as inputs:
  pinMode(left, INPUT);
  pinMode(middle, INPUT);
  pinMode(right, INPUT);

  byte_1 = byte_1 & 0x08; // Bit 3 is always 1

}

// Run indefinitely on loop
void loop() {

  // read the state of the pushbutton value:
  leftState = digitalRead(left);
  middleState = digitalRead(middle);
  rightState = digitalRead(right);

  // Check the value of the left mouse button
  if (leftState == HIGH) {
    // Debugging
    Serial.print("Left is high!");
  } else {
    // Debugging
    Serial.print("Left is low!");
  }

  delay(100); // Delay measured in ms
}
