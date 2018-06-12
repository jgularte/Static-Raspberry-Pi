/* \file       diagpov.c
\author     Eddy Ferre - ferree@seattleu.edu
\date       01/13/2018

\brief      Main diagnostic file for the static RPi controllers..

This program sets up various sensors and actuators, then allows the user to interact from the console.
*/

// Linux C libraries
#include <stdio.h>     //printf, fprintf, stderr, fflush, scanf, getchar
#include <string.h>    //strncpy, strerror
#include <errno.h>     //errno
#include <stdlib.h>    //exit, EXIT_SUCCESS, EXIT_FAILURE
#include <signal.h>    //signal, SIGINT, SIGQUIT, SIGTERM
#include <wiringPi.h>  //wiringPiSetup, pinMode, delay, INPUT, OUTPUT, PWM_OUTPUT
#include <softPwm.h>
#include <sys/time.h>  //gettimeofday, timeval

// Local headers
#include "static_pinout.h"
#include "web_client.h"
#include "motor.h"

// define pins


volatile int pulseCounter, currentPulseCounter;

// Local function declaration
/// Function controlling exit or interrupt signals
void control_event(int sig);

/// Test function for the motor encoders sensors
int encoderTest(void);

/// Test function for the motor drive and encoders sensors
int timedMotorDriveTest();

/// Test function for the motor PWM values
int rampUpPwmTest(void);

/// Test function for the web client transmission
void messagingTest(void);

/// Encoder Interrupt
void encoderUpt (void)
{
    ++pulseCounter;
}

/* static long time_diff_us(struct timeval end , struct timeval start)
{
    return (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
}
 */
/*
main function - Entry point function for diagnostic

@param argc number of arguments passed
@param *argv array of string argumnents

@return number stdlib:EXIT_SUCCESS exit code when no error found.
*/

int main (int argc, char *argv[])
{
    // Inform OS that control_event() function will be handling kill signals
    (void)signal(SIGINT,control_event);
    (void)signal(SIGQUIT,control_event);
    (void)signal(SIGTERM,control_event);

    // Initialize the Wiring Pi facility
    if (wiringPiSetup() != 0)
    {
        // Handles error Wiring Pi initialization
        fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
	
	wiringPiISR(ENC_PIN, INT_EDGE_RISING, &encoderUpt); //Starts ISR pulse counter of encoder signal
	
	pinMode(5, OUTPUT);
	pinMode(4, OUTPUT);
	digitalWrite(5, 0);
	digitalWrite(4, 0);
	
	
    // Parse the function argument
    int test_num;
    char c;
    if(argc < 2)
    {
        printf("  Select one of the test below:\n");
        printf("    0 - Encoder Test\n");
        printf("    1 - Timed Motor Drive Test\n");
        printf("    2 - Ramp-Up PWM Test\n");
        printf("    3 - Messaging Test\n");
        printf("  Type a test number then press ENTER... > ");
        fflush(stdout);
        scanf(" %c", &c);
        getchar();
        fflush(stdin);
    }
    else
    {
        c = argv[1][0];
    }

    if(c < '0' || c > '3')
    {
        // Handles argument type error
        fprintf(stderr, "Argument must be a number, received %c\n", c);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    test_num = c - '0';
	// sig quit
	//(void)signal(SIGQUIT,control_event);
	//(void)signal(SIGINT,control_event);
    // Select the test to run
    switch(test_num)
    {
    case 0:
        encoderTest();
        break;
    case 1:
        timedMotorDriveTest();
        break;
    case 2:
        rampUpPwmTest();
        break;
    case 3:
        messagingTest();
        break;
    default:
        fprintf(stderr, "Cannot handle argument \"%s\" (=%d)\n", argv[1], test_num);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    printf("Test completed\n");
    return EXIT_SUCCESS;
}
 
/*
encoderTest function

Initialize the motor encoder ISR and monitors the encoder counter.
Displays the counter value

@return number '0' exit code when no error found.
*/


int encoderTest(void)
{
    printf("Running encoder Test\n");
	
	
    /// \todo
    /// * Initialize motor encoder (GPIO pin mode, GPIO pins) DONE
    /// * Display initial counter value
	
    /// * Console instruction to move the motor rotor until 'ENTER' key pressed
	initMotor();

	int stop = 0;
	
	
	while (stop < 1) {
		
		// if enter is pressed
		if (getchar() == '\n') {
			printf("user pressed enter");
			stop++;
		}
	} 
	
    /// * Display the new counter value in the console
	stopMotor();
	printf("%s\n", "");
	printf(" The pulse counter is %d\n", pulseCounter);

    /// * Display the motor RPM    
	printf(" RPM: %d\n", getRPM());
    return 0;
}

/*
timedMotorDriveTest function

Initialize the motor encoder ISR and monitors the encoder counter.
Displays the counter value of the motor encoder
Displays the motor RPM

@return number '0' exit code when no error found.
*/
int timedMotorDriveTest(void)
{
       printf("Running Timed Motor Drive Test\n");

 

    /// \todo

    /// * Initialize motor encoder (variables, GPIO pins)

   initMotor();

    /// * Display the current encoder count

   printf("Initial Encoder Count: %d\n", getCount());

   /// * Set the motor PWM to 100% duty-cycle

   setDutyCycle(100);

   /// * Wait for 2 seconds

    delay(2000);

    /// * Display the new counter value in the console

   printf("Final Encoder Count: %d\n", getCount());

    /// * Display the motor RPM

   printf("RPM: %d\n", getRPM());

    /// * Stop the motor

   stopMotor();

 

    return 0 ;

    return 0 ;
}

/*
rampUpPwmTest function

Initialize the motor encoder ISR and monitors the encoder counts.
Increments the motor duty-cycle by 5% every time 'ENTER' key is pressed.

@return number '0' exit code when no error found.
*/

int rampUpPwmTest(void)
{
    printf("Running Ramp Up PWM Test\n");

    /// \todo
    /// * Initialize motor encoder (variables, GPIO pins)
    /// * Until the ducty cycle (DC) is 100%, or 'q' is entered, repeat:
    ///   - Incremement DC by 5%
    ///   - wait until the "ENTER" key pressed
    /// * Stop the motor

    return 0;
}


void messagingTest(void)
{
    char ip[20], message[MESSAGE_BUFFER_SIZE];
    printf("Enter the IP to send to > ");
    fflush(stdout);
    scanf ("%s", ip);
    printf("Initialize Web Client\n");
    initWebClient(ip);
    sprintf(message, "%s,%s,display", getMyIP("wlan0"),ip);
    printf("Send a message: \"%s\"\n", message);
    sendMessage(message);
    printf("Waiting for a response...\n");
    const char* response = getMessage();
    printf("Received: \"%s\"\n", response);
}


void control_event(int sig)
{
    printf("\b\b  \nExiting diagpov... ");

    //stop the motor
    stopMotor();
    delay(200);
    printf("Done\n");
    exit(EXIT_SUCCESS);
}
