/**
\file       pov.c
\author     Eddy Ferre - ferree@seattleu.edu
\date       01/13/2018

\brief      Main persistence of vision file for the static RPi controllers.

This program will run the logic of the static RPi
*/

// Linux C libraries
#include <stdio.h>	//printf, fprintf, stderr, fflush, scanf, getchar
#include <string.h>   //strncpy, strerror
#include <errno.h>	//errno
#include <stdlib.h>   //exit, EXIT_SUCCESS, EXIT_FAILURE
#include <signal.h>   //signal, SIGINT, SIGQUIT, SIGTERM
#include <wiringPi.h> //wiringPiSetup, pinMode, digitalWrite, delay, INPUT, OUTPUT, PWM_OUTPUT
#include <time.h>	 // time

// Local headers
#include "static_pinout.h"
#include "motor.h"		//initMotor
#include "web_client.h" //initWebClient_new_port

// Local function declaration
/// Function controlling exit or interrupt signals
void control_event(int sig);

/**
main function - Entry point function for pov

@param argc number of arguments passed
@param *argv IP address of the rotor RPi

@return number stdlib:EXIT_SUCCESS exit code when no error found.
*/

int main(int argc, char *argv[])
{
	
	// Create Interupts
	(void)signal(SIGINT, control_event);
	(void)signal(SIGQUIT, control_event);
	(void)signal(SIGTERM, control_event);

	//Local variable definition
	char rotor_ip[20] = "10.133.0.23";

	// Initialize the Wiring Pi facility
	printf("Initialize Wiring Pi facility... ");
	if (wiringPiSetup() != 0)
	{
		// Handles error Wiring Pi initialization
		fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
		fflush(stderr);
		exit(EXIT_FAILURE);
	}
	printf("Done\n");

	// Initialize GPIO pins
	printf("Initialize GPIO pins... ");
	pinMode(27, OUTPUT);
	digitalWrite(27, 0);
	system("gpio mode 23 alt0");
	system("gpio mode 26 alt5");

	/// \todo
	/// * Motor controller (Separate Thread ot Main loop)
	/// * Display controller (Separate Thread ot Main loop)
	
	// Default Hex Display
	int clock[]={	0x800000,	0xAE0000,	0xCA0000,	0xB20000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0xBE0000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x7E0000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x900000,	0xA80000,	0xA00000,	0x900000,	0xA00000,	0xA80000,	0x900000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0xBE0000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0xBE0000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x988000,	0xA44000,	0xA44000,	0x9F8000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0xBE0000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0xBE0000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x9C0000,	0x840000,	0x8C0000,	0x940000,	0x940000,	0x880000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0xBE0000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0xBE0000,	0x800000,	0x800000,	0x800000,	0x800000,	0x800000,	0xA20000,	0xFE0000,	0x820000};

	// Connect to Server
	printf("  rotor RPi IP     : %s\n", rotor_ip);
	printf("  My wireless IP is: %s\n", getMyIP("wlan0"));
	int array[120];
	char ip[20], buffer[MESSAGE_BUFFER_SIZE];
	printf("Enter the IP to send to > ");
	fflush(stdout);
	scanf("%s", ip);
	printf("Initialize Web Client\n");

	initWebClient(ip);

	// Start encoder counter ISRs, setting actual motor RPM
	printf("Initialize Motor... ");
	initMotor();
	printf("Done\n");

	// Time is dealt with here
	int min, prev_min = -1;
	int hour;
	time_t current_time;
	struct tm *pretty_time;

	while (1)
	{
		
		// Get and format time correctly
		time(&current_time);
		pretty_time = localtime(&current_time);
		min = pretty_time->tm_min;
		hour = pretty_time->tm_hour;
		
		
		// only enter if it is a new minute
		if (min != prev_min)
		{
			
			// update the display array
			prev_min = min;
			for (int i = 0; i < 120; i++)
			{
				array[i] = clock[i];
			}
			
			// formulas used to update hour and minute hand
			int minInd = min * 2;
			int hourInd = (hour * 10 + min / 6) % 120;
			
			// add the hands into the new array
			array[minInd] |= 0x00FFFF;
			array[hourInd] |= 0x000FFF;

			sprintf(buffer, "%s,%s,display", getMyIP("wlan0"), rotor_ip);
			for (int i = 0; i < 120; i++)
			{				
				// concatenate the string in the correct format so that the server will accept it
				sprintf(buffer, "%s,%d,%X", buffer, i, array[i]);				
			}
			
			// send the message
			sprintf(buffer, "%s\n", buffer);
			printf("Send a message: \"%s\"\n", buffer);
			sendMessage(buffer);
			
			// if we are the top of the hour, ring the bells
			if(min == 0)
			{
				digitalWrite(27, 1);
				system("omxplayer -o local res/hourlychimebeg.mp3 > /dev/null 2>&1");
				int num_bell = hour % 12;
				num_bell = num_bell ? num_bell : 12;
				while(num_bell--)
				{
					system("omxplayer -o local res/bigbenstrike.mp3 > /dev/null 2>&1");
				}
				digitalWrite(27, 0);
			}
		}
	}

	return 0;
}


// keyboard interrupt function
void control_event(int sig)
{
	printf("\b\b  \nExiting pov... ");

	//stop the motor
	stopMotor();

	delay(200);
	printf("Done\n");
	exit(EXIT_SUCCESS);
}
