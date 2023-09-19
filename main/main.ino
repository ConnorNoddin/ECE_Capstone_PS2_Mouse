// Connor Noddin
// ECE 406
// Computer Engineering Capstone

// GPIO pin assignments for mouse buttons
const int left = 5;  // Left mouse button
const int middle = 6;    // Middle mouse button
const int right = 7; // Right mouse button

int leftState, middleState, rightState; // If button is pressed or not

//Initial setup, run on boot
void setup() {

  //Initiate Serial communication.
  Serial.begin(9600); //9600 its bits/second (Baud rate)

  // Initialize the mouse buttons as inputs:
  pinMode(left, INPUT);
  pinMode(middle, INPUT);
  pinMode(right, INPUT);

  // Built in LED for debugging
  pinMode(LED_BUILTIN, OUTPUT);

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
    digitalWrite(LED_BUILTIN, HIGH);

  } else {
    // Debugging
    Serial.print("Left is low!");
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(100); // Delay measured in ms
}
