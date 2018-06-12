#ifndef STATIC_PINOUT_H
#define STATIC_PINOUT_H

/**
\file       static_pinout.h
\brief      GPIO Pin file of the static RPi.

This file maps all GPIO pins used by the hardware and RPi of the static portion of the POV
*/

// GPIO Input Pins
/// Email-My-IP and utility switch "S1" (input pin 37, GPIO_25)
#define IN_SW1 25

// GPIO Output Pins
/// Motor enable signal (output pin 10, GPIO_16)
#define OUT_MT_EN 16
/// Motor PWM drive (output pin 12, PWM_0/GPIO_1)
#define OUT_PWM 4

// #define PWM_PIN 	4
#define PWM_PIN 4
#define DIS_PIN 5
#define ENC_PIN 6
#define DIR_PIN 7
#define PULSE_SAMPLING_MS 250
#define MAX_PWM 1024


#endif
