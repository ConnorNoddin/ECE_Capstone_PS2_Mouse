// Connor Noddin
// ECE 406
// Computer Engineering Capstone

#define R_BUTTON 0x02
#define L_BUTTON 0x01
#define M_BUTTON 0x04

#define RESET 0xff

// GPIO pin assignments for mouse buttons
const int left = 2;  // Left mouse button
const int middle = 3;    // Middle mouse button
const int right = 4; // Right mouse button

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
    Serial.print("Left is high!\n");
  } else {
    // Debugging
    Serial.print("Left is low!\n");
  }

    // Check the value of the right mouse button
  if (rightState == HIGH) {
    // Debugging
    Serial.print("Right is high!\n");
  } else {
    // Debugging
    Serial.print("Right is low!\n");
  }

    // Check the value of the middle mouse button
  if (middleState == HIGH) {
    // Debugging
    Serial.print("Middle is high!\n");
  } else {
    // Debugging
    Serial.print("Middle is low!\n");
  }

  delay(1000); // Delay measured in ms
}
