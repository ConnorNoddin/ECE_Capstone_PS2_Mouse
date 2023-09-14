// GPIO pin assignments for mouse buttons
const int left = 5;  // Left mouse button
const int middle = 6;    // Middle mouse button
const int right = 7; // Right mouse button

void setup() {
  // initialize the mouse buttons as inputs:
  pinMode(left, INPUT);
  pinMode(middle, INPUT);
  pinMode(right, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

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


  sleep(100)
}
