// Connor Noddin
// ECE 406
// Computer Engineering Capstone
// ps2mouse.h

#ifndef ps2mouse_H_  //Guard
#define ps2mouse_H_

#include <Arduino.h>

// Packet Manipulation
#define R_BUTTON 0x02
#define L_BUTTON 0x01
#define M_BUTTON 0x04
#define X_SIGN 0x10
#define Y_SIGN 0x20
#define X_OVERFLOW 0x40
#define Y_OVERFLOW 0x80
#define BYTE_1_BIT 0x08

// PS/2 Commands
#define RES 0xFF  //Reset
#define RESEND 0xFE
#define DEF 0xF6  //Default
#define DISABLE 0xF5
#define ENABLE 0xF4
#define SET_RATE 0xF3
#define GET_DEV_ID 0xF2
#define REMOTE 0xF0
#define WRAP 0xEE
#define RES_WRAP 0xEC
#define READ 0xEB
#define STREAM 0xEA
#define STATUS_REQ 0xE9
#define SET_RES 0xE8
#define SCALE_2 0xE7
#define SCALE_1 0xE6

#define BAT 0xAA
#define ACK 0xFA
#define ID 0x00
#define STAT_1 0x02
#define STAT_2 0x64


// Misc Setup
#define FORCE_ENABLE 0    //Good for debugging data packets. Will make most hosts not work
#define CPI 1000          // Counts per inch of sensor
#define SERIAL_RATE 9600  // Bits/second

//Timings - This is what affects the performance and stability of the mouse
#define INIT_DELAY 2500  //Delay before sending first packets... 2500 for USB adapter. 500 for motherboard
#define CLOCK_HALF 20    //10 - 16.7khz clock is standard
#define CLOCK_FULL 40
#define BYTE_DELAY 3000    //1500 works good. Higher is more stable
#define HOST_TIMEOUT 1000  //30 is default
#define TIMEOUT 1000
#define DATA_DELAY 4  // Lower is smoother but less stable

#define SS 10  // SS pin on arduino. For nano 10 is default. Uno 3 is default

// GPIO pin assignments for mouse buttons
#define LEFT 2    // Left mouse button
#define MIDDLE 3  // Middle mouse button
#define RIGHT 4   // Right mouse button

// GPIO pin assignments for PS/2 connection
#define DATA_IN_A A2  //normally 8
#define DATA_IN 8
#define CLK_IN 7
#define DATA_OUT 6
#define CLK_OUT 5

int ps2_clock(void);

int ps2_dwrite(byte ps2_Data);

int ps2_dread(byte* read_in);

int ps2_command(byte input);

void ack();

int parity(byte p_check);

byte get_button_states(void);

byte* get_bytes(void);

#endif  // ps2mouse_H_
