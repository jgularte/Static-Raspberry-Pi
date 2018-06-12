#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <pthread.h>
#include <softPwm.h>

#include "motor.h"
#include "static_pinout.h"
#include "utils.h"

// Local function declaration
void *rpmCalculatorThrFunc(void *null_ptr);

pthread_t rpmCalculatorThr;
static int init_motors_done = 0;
float rpm;
volatile int pulseCounter, currentPulseCounter;

// ISR function for the encoder counter
void encoderISR(void)
{
    ++pulseCounter;
}

// Thread function to calculate RPM
void *rpmCalculatorThrFunc(void *null_ptr)
{
    int value = 0;
    float rpm = 1500;
    const int expected_cnt = (int)(rpm * 211.2 * (float)PULSE_SAMPLING_MS / (4000.0 * 60.0));

    while (1)
    {
        pulseCounter = 0;
        delay(PULSE_SAMPLING_MS); //Always keep a sleep or delay in infinite loop
        currentPulseCounter = pulseCounter;
        value += (int)((float)(expected_cnt - currentPulseCounter));
        value = value > MAX_PWM ? MAX_PWM : (value < 0 ? 0 : value); // Bounds 0 <= value <= MAX_PWM
        softPwmWrite(PWM_PIN, value);
        // dutyCycle = (float)value * 100.0 / (float)MAX_PWM;
        // rpm = count * 240 / 211.2 * 1000 / rpm_sampling_ms;
        rpm = (float)currentPulseCounter * 4000.0 * 60.0 / (211.2 * (float)PULSE_SAMPLING_MS);
        // printf("\nFinal RPM: %.2f\n", rpm);
        // TODO:
        // Calculate RPM from count and diff_time_us (in microseconds)
        // Hint: look at fall quarter Lab 7 or Lab 8
        // rpm = ???
        // *** Note *** The line below is useless, it prevent a "Wunused-but-set-variable" warning.
        // To be removed once use to calculate rpm
    }
}

// init_motor: initializes motor pin (GPIO allocation and initial values of output)
// and initialize the elements of all motor control data structure
int initMotor(void)
{
    rpm = 0;
    pulseCounter = 0;

    // Initialize GPIO pins
    pinMode(ENC_PIN, INPUT);
    pinMode(DIS_PIN, OUTPUT);
    pinMode(PWM_PIN, OUTPUT);
    digitalWrite(DIS_PIN, 0); // Set disable pin to low
    digitalWrite(PWM_PIN, 1);
    digitalWrite(DIR_PIN, 1); // set direction, 1: spin clockwise, 0: spin counter-clockwise

    if (softPwmCreate(PWM_PIN, 0, MAX_PWM)) //Create a SW PWM, value from 0 to MAX_PWM (=100% duty cycle)
    {
        fprintf(stderr, "Error creating software PWM: %s\n", strerror(errno));
        fflush(stderr);
        return -1;
    }

    if (!init_motors_done)
    {
        // Start counter ISR and RPM calculator
        wiringPiISR(ENC_PIN, INT_EDGE_FALLING, &encoderISR);
        int ret = pthread_create(&(rpmCalculatorThr), NULL, rpmCalculatorThrFunc, NULL);
        if (ret)
        {
            fprintf(stderr, "Error creating rpmCalculatorThr - pthread_create() return code: %d\n", ret);
            fflush(stderr);
            return ret;
        }
    }
    init_motors_done = 1;
    return 0;
}

// stopMotor: stop the motor
int stopMotor(void)
{
    softPwmWrite(PWM_PIN, 0);
    pinMode(PWM_PIN, OUTPUT);
    digitalWrite(PWM_PIN, 0);
    return 0;
}

// getCount: accessor funtion of a motor encoder counter
int getCount(void)
{
    return pulseCounter;
}

// getRPM: accessor funtion of a motor encoder counter
int getRPM(void)
{
    return rpm;
}

// setDutyCycle: sets the motor PWM duty-cycle
void setDutyCycle(int dutyCycle)
{
    int dc;
    if (dutyCycle <= 0)
    {
        stopMotor();
    }
    else
    {
        dc = dutyCycle <= 100 ? dutyCycle : 100;
        digitalWrite(OUT_MT_EN, 1);
        pwmWrite(PWM_PIN, (dc * 1024) / 100);
    }
}
