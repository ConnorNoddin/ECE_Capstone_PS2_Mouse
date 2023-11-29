// Connor Noddin
// ECE 406
// Computer Engineering Capstone
// ADNS3050.h

#ifndef ADNS3050_H_  //Guard
#define ADNS3050_H_

// Pins used. These are default for the ATMEGA
#define PIN_SCLK 13
#define PIN_MISO 12
#define PIN_MOSI 11
#define PIN_NCS 10    //Digital Pin 10... SS
#define PIN_MOTION 9  //Digital Pin 9 is free

// All registers for ADNS3050
#define PROD_ID 0x00
#define REV_ID 0x01
#define MOTION_ST 0x02
#define DELTA_X 0x03
#define DELTA_Y 0x04
#define SQUAL 0x05
#define SHUT_HI 0x06
#define SHUT_LO 0x07
#define PIX_MAX 0x08
#define PIX_ACCUM 0x09
#define PIX_MIN 0x0a
#define PIX_GRAB 0x0b
#define MOUSE_CTRL 0x0d
#define RUN_DOWNSHIFT 0x0e
#define REST1_PERIOD 0x0f
#define REST1_DOWNSHIFT 0x10
#define REST2_PERIOD 0x11
#define REST2_DOWNSHIFT 0x12
#define REST3_PERIOD 0x13
#define PREFLASH_RUN 0x14
#define PREFLASH_RUN_DARK 0x18
#define MOTION_EXT 0x1b
#define SHUT_THR 0x1c
#define SQUAL_THRESHOLD 0x1d
#define NAV_CTRL2 0x22
#define MISC_SETTINGS 0x25
#define RESOLUTION 0x33
#define LED_PRECHARGE 0x34
#define FRAME_IDLE 0x35
#define RESET 0x3a
#define INV_REV_ID 0x3f
#define LED_CTRL 0x40
#define MOTION_CTRL 0x41
#define AUTO_LED_CONTROL 0x43
#define REST_MODE_CONFIG 0x45

void com_start();

byte Read(byte reg_addr);

void Write(byte reg_addr, byte data);

void startup();

int convTwosComp(int b);

int getX();

int getY();

#endif  // ADNS3050_H_
