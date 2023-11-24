// Connor Noddin
// ECE 406
// Computer Engineering Capstone

#include <SPI.h>
#include <avr/pgmspace.h>
#include "PMW3360.h"
#include "ps2mouse.h"

// Allows device to send packets
int DEVICE_ENABLED = 0;  //Flag for if host sent device enabled signal

// SPI mouse sensor
PMW3360 sensor;

/*
Description: Initial setup, runs on boot only once.
Establishes GPIO pins and sensor. Also sends initial packet sequence
*/
void setup() {

  int ret;

  //Initiate Serial communication for debugging
  Serial.begin(SERIAL_RATE);  //9600 bits/second (Baud rate)

  delay(INIT_DELAY);  // 500 ms delay for PS/2 standard

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

  //Check if sensor initialized successfully
  if (sensor.begin(SS))  // Pin 10 on arduino nano
    Serial.println("Sensor initialization success");
  else
    Serial.println("Sensor initialization fail");

  sensor.setCPI(CPI);  // Sets mouse resolution/sensitivity

  // Write self test passed
  while (ps2_dwrite(BAT) != 0)
    ;
  // Write mouse ID
  while (ps2_dwrite(ID) != 0)
    ;
}

/*
Description: Runs indefinitely. Establishes handshake with PS2_host.
Then, reads sensor and buttons. Finally sends 3 data packets to PS2_host.
*/
void loop() {
  // Bit 3 is always 1 for byte 1
  byte byte_1 = BYTE_1_BIT, byte_2, byte_3;  // 3 bytes for PS/2 packet

  byte tmp;  //Temporary byte from functions

  int16_t sensor_x, sensor_y;  //Sensor x and y movement
  byte x_sign, y_sign;         // Bytes just for memory efficiency

  int ret;

  // Check if host is trying to send commands
  if (((digitalRead(DATA_IN) == LOW) || (digitalRead(CLK_IN) == LOW)) && DEVICE_ENABLED == 0) {
    while (ps2_dread(&tmp))
      ;  // If this fails it halts the program
    ps2_command(tmp);
    if (tmp == ENABLE) DEVICE_ENABLED = 1;
  }

  tmp = get_button_states();  // Gets state of all three buttons

  byte_1 = byte_1 | tmp;  //Saves states to byte 1

  // Code for sensor
  PMW3360_DATA data = sensor.readBurst();  //Get data
  sensor_x = data.dx;                      //Extract change in x
  sensor_y = data.dy;                      //Extract change in y

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

  // Writes data to host
  if (DEVICE_ENABLED == 1 || FORCE_ENABLE == 1) {
    ps2_dwrite(byte_1);
    ps2_dwrite(byte_2);
    ps2_dwrite(byte_3);

    delay(DATA_DELAY);  // Delay between bytes. Increases stability

    //Debugging
    /*
    Serial.print("Byte 1: 0x");
    Serial.print(byte_1, HEX);
    Serial.print("\t Sensor X: ");
    Serial.print(sensor_x, DEC);
    Serial.print("\t Sensor Y: ");
    Serial.print(sensor_y, DEC);
    Serial.print("\n");
    */
  }
}