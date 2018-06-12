#ifndef MOTOR_H
#define MOTOR_H

/**
\file       motor.h
\brief      Motor Driver file.

This file provides functions to handle the motor and motor encoders.
*/

/// @brief Initialize motor pins, encoder counter ISR and RPM calulator thread
int initMotor(void);

/// @brief Stop the motor
int stopMotor(void);

/// @brief Fecth the current encoder count
/// @return motor encounter value
int getCount(void);

/// @brief Fecth the current motor RPM
/// @return motor encounter value
/// @todo Calculate RPM from count and diff_time_us (in microseconds) in motors.c:rpmCalculatorThrFunc
int getRPM(void);

/// @brief Set the duty-cycle of the motor
/// @param dutyCycle Percent duty-cycle
void setDutyCycle(int dutyCycle);

#endif
