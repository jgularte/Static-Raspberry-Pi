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
	//pinMode(IN_SW1, INPUT);
	//pinMode(IN_MT_ENC_A, INPUT);
	//pinMode(IN_MT_ENC_B, INPUT);
	//pinMode(OUT_MT_EN, OUTPUT);
	pinMode(27, OUTPUT);
	digitalWrite(27, 0);
	system("gpio mode 23 alt0");
	system("gpio mode 26 alt5");
	//digitalWrite(OUT_MT_EN, 0);
	//printf("Done\n");

	/// \todo
	/// * Motor controller (Separate Thread ot Main loop)
	/// * Display controller (Separate Thread ot Main loop)

	const int clock[]={	0x000000,	0x2E0000,	0x4A0000,	0x320000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x100000,	0x280000,	0x200000,	0x100000,	0x200000,	0x280000,	0x100000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x188000,	0x244000,	0x244000,	0x1F8000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x1C0000,	0x040000,	0x0C0000,	0x140000,	0x140000,	0x080000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x000000,	0x220000,	0x7E0000,	0x020000};


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

		time(&current_time);
		pretty_time = localtime(&current_time);
		min = pretty_time->tm_min;
		hour = pretty_time->tm_hour;

		if (min != prev_min)
		{

			prev_min = min;
			for (int i = 0; i < 120; i++)
			{
				array[i] = clock[i];
			}

			int minInd = min * 2;
			int hourInd = (hour * 10 + min / 6) % 120;

			array[minInd] |= 0x00FFFF;
			array[hourInd] |= 0x000FFF;

			sprintf(buffer, "%s,%s,display", getMyIP("wlan0"), rotor_ip);
			for (int i = 0; i < 120; i++)
			{
				//	if (array[i] != 0)
				//	{
				sprintf(buffer, "%s,%d,%X", buffer, i, array[i]);
				//}
			}
			sprintf(buffer, "%s\n", buffer);
			printf("Send a message: \"%s\"\n", buffer);
			sendMessage(buffer);

			if(min == 21)
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

	printf("Waiting for a response...\n");
	const char *response = getMessage();
	printf("Received: \"%s\"\n", response);

	return 0;
}

void control_event(int sig)
{
	printf("\b\b  \nExiting pov... ");

	//stop the motor
	stopMotor();

	delay(200);
	printf("Done\n");
	exit(EXIT_SUCCESS);
}
