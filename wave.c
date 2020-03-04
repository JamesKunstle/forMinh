/****************************************************/
/*                                                  */
/*   CS-454/654 Embedded Systems Development        */
/*   Instructor: Renato Mancuso <rmancuso@bu.edu>   */
/*   Boston University                              */
/*                                                  */
/*   Description: template file for digital and     */
/*                analog square wave generation     */
/*                via the LabJack U3-LV USB DAQ     */
/*                                                  */
/****************************************************/

#include "u3.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
/* This function should initialize the DAQ and return a device
 * handle. The function takes as a parameter the calibratrion info to
 * be filled up with what obtained from the device. */

timer_t timer1_digital, timer2_analog;
struct sigaction digital_sigaction_struct, analog_sigaction_struct; // init the sig actions and sigevents in global scope
struct sigevent digital_sigevent_struct, analog_sigevent_struct;

sigset_t mask, wait_mask; //init the masks that we will use for blocking / waiting. type from demo

struct itimerspec timer_period;
struct timespec res;

void digital_wave_handler();
void analog_wave_handler();
void setup_digital_wave_signal();
void setup_digital_wave_event_handler();
void setup_analog_wave_signal();
void setup_digital_wave_event_handler();
void setup_timer_period(float frequency);
void start_waves();

//this is the handler for when the SIGRTMIN comes in
void timer1_sighandler (int signo)
{
		(void)signo;
        printf("Timer1 signal\n");
}

void handle_term (int signo)
{
        (void)signo;
        printf("\nYeah, I am not doing that. Go terminate yourself.\n");
}


void analog_wave_handler()
{
	printf("analog wave handler fired\n");
}

void digital_wave_handler()
{
	printf("digital wave handler fired\n");
}
HANDLE init_DAQ(u3CalibrationInfo * caliInfo)
{
	HANDLE hDevice;
	int localID;
	
	/* Invoke openUSBConnection function here */
	
	/* Invoke getCalibrationInfo function here */
	
	return hDevice;
}

void setup_digital_wave_event_handler()
{
	digital_sigevent_struct.sigev_notify = SIGEV_SIGNAL; // set the timer to send a signal
	digital_sigevent_struct.sigev_signo = SIGRTMIN; //the signal that is being sent
        digital_sigevent_struct.sigev_value.sival_int = 0;

	/*
 	*Once this finishes, then we have the first timer and its event set up.
 	* */
}

void setup_digital_wave_signal()
{
	//setup similarly to class demo
	digital_sigaction_struct.sa_flags = SA_SIGINFO;
	//set the handler function
	digital_sigaction_struct.sa_sigaction = digital_wave_handler;
	
	//clear the masks of any values
	sigemptyset(&mask);
        sigemptyset(&wait_mask);
        sigemptyset(&digital_sigaction_struct.sa_mask);
	
	//add the SIGRTMIN and SIGRTMAX to the digital_sigaction_struct to the signal set 
	sigaddset(&digital_sigaction_struct.sa_mask, SIGRTMIN);
        sigaddset(&digital_sigaction_struct.sa_mask, SIGRTMAX);			

	
	setup_digital_wave_event_handler(); //now we need to set the event for the signal interception
}

void setup_analog_wave_event_handler()
{
	//setup the event for the analog wave to work with signals:
	analog_sigevent_struct.sigev_notify = SIGEV_SIGNAL;
        analog_sigevent_struct.sigev_signo = SIGRTMAX;
	//make sure that the signal that it sends is SIGRTMAX
	analog_sigevent_struct.sigev_value.sival_int = 0;
}

void setup_analog_wave_signal()
{
	sigemptyset(&analog_sigaction_struct.sa_mask);
	sigaddset(&analog_sigaction_struct.sa_mask, SIGRTMAX);
        analog_sigaction_struct.sa_flags = SA_SIGINFO;
        analog_sigaction_struct.sa_sigaction = analog_wave_handler;

	setup_analog_wave_event_handler();		
}

// setup for the timer
void setup_timer_period(float frequency)
{	//Because the timer is in global scope, we can set it up here in this function
	//memset(&timer1_event, 0, sizeof(timer1_event)); this was from the manual but I don't know if I need it
	timer_period.it_interval.tv_sec = (int)(1 / frequency);
	timer_period.it_interval.tv_nsec = ((1 / frequency) - timer_period.it_interval.tv_sec) * 500000000UL;
	//we have the period of the timer set according to the frequency, now we need the value not set to 0.
	timer_period.it_value.tv_sec = 0;
	timer_period.it_value.tv_nsec = 1; 
	//but the ns interval for the value 
}


void start_waves()
{
	sigaddset(&mask, SIGRTMAX);
	sigaddset(&mask, SIGRTMIN);
	//add both of the signals to the mask so that we can unblock them
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	//unblock the signals so that when we start the timers, we'll get signals to handle.
	
	printf("Timers started\n");
	timer_settime(timer1_digital, 0, &timer_period, NULL);
    timer_settime(timer2_analog, 0, &timer_period, NULL);	
}


int main(int argc, char **argv)
{

	/* Invoke init_DAQ and handle errors if needed */
	

	/* Provide prompt to the user for a voltage range between 0
	 * and 5 V. Require a new set of inputs if an invalid range
	 * has been entered. */
	while(1)
	{	
		int usr_int_input,usr_int_input2;
		printf("Enter the upper bound (max=5):\n");
		int int_result = scanf("%d", &usr_int_input);
		if(int_result == EOF)
		{
 			printf("Error in getting input from user!\n");
			continue;
		}
		else
		{	
			printf("User entered %d\n", usr_int_input);
			if((usr_int_input > 5) || (usr_int_input < 0))
			{
				printf("The input was out of bounds- please input a valid voltage.\n");
				continue;
			}
		}

		printf("Enter the lower bound (min=0):\n");
		int int_result2 = scanf("%d", &usr_int_input2);
		if(int_result2 == EOF)
		{
 			printf("Error in getting input from user!\n");
			continue;
		}
		else
		{	
			printf("User entered %d\n", usr_int_input2);
			if((usr_int_input2 > 5) || (usr_int_input2 < 0))
			{
				printf("ERROR: The input was out of bounds- please input a valid voltage.\n");
				continue;
			}
			else if (usr_int_input <= usr_int_input2)
			{
				printf("ERROR: Lower bound is larger or equal than upper bound.\n");
				continue;
			}
			else
			{
				break;
			}
		}

	}	
	/* Compute the maximum resolutiuon of the CLOCK_REALTIME
	 * system clock and output the theoretical maximum frequency
	 * for a square wave */
	
	//JSK if 0, then the set has failed.
	int clock_check =  clock_getres(CLOCK_REALTIME, &res);
	if(!clock_check)
	{
		printf("clock_getres() succeeded!\n");
	}
	//res.tv_nsec = 15; // JSK TODO this is set to 15 because I don't trust what I read from the tv_nsec	
	printf("Period of timer-checking on this machine: %luns\n", res.tv_nsec);	
	printf("Freq: %luHz\n", 1000000000/res.tv_nsec);	
	




	/* Provide prompt to the user to input a desired square wave
	 * frequency in Hz. */
	printf("Please enter a frequency in Hz for the square wave:\n");
	float usr_float_input;
	int float_result = scanf("%f", &usr_float_input);

	if(float_result == EOF)
	{
		printf("Error in getting input from the user!\n");
	}
	else
	{
		printf("User entered: %f\n",  usr_float_input);
	}


	
	/* Program a timer to deliver a SIGRTMIN signal, and the
	 * corresponding signal handler to output a square wave on
	 * BOTH digital output pin FIO2 and analog pin DAC0. */

	setup_digital_wave_signal(); // this sets up the timer event and the signal action
	timer_create(CLOCK_REALTIME, &digital_sigevent_struct, &timer1_digital);
	sigaction(SIGRTMIN, &digital_sigaction_struct, NULL);


	setup_analog_wave_signal();	
	timer_create(CLOCK_REALTIME, &analog_sigevent_struct, &timer2_analog);
        sigaction(SIGRTMAX, &analog_sigaction_struct, NULL);

	//We have the signals blocked. We need unblock the signals, then start the timers
	
	start_waves();

	/* The square wave generated on the DAC0 analog pin should
	 * have the voltage range specified by the user in the step
	 * above. */
	
	/* Display a prompt to the user such that if the "exit"
	 * command is typed, the USB DAQ is released and the program
	 * is terminated. */

	while(1)
	{


	}

	return 0;
}

