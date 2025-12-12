/*
File:			main.c
Author:			Miller DeMark, built off Ethan Schlepp's version
Description:	This file runs our binary counter on the ARM processor. It runs a counter from the moment
				power is given to the board, then resets the counter and pauses it once the counter is 
				connected to a UART Terminal, i.e. PuTTy or TeraTerm. From there, the user can enter 
				commands to control the controller. 
				-STOP = stops counting
				-COUNT = starts counting from the value the board is at
				-SET = sets a value that the user defines
				-SPEED = changes how fast the counter counts, defaults to 200ms
				-HELP = gives a list of commands for the user
*/

#include "atmel_start.h"
#include "usb_start.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Command handlers
#define CMD_BUFFER_SIZE 64
char usb_cmd_buffer[CMD_BUFFER_SIZE];
uint8_t usb_cmd_index = 0;

// Variable declarations 
uint32_t count = 1023;		// Sets count to 1023 whenever it starts, visual confirmation that the lights are working
uint32_t speed = 200;		// Counter speed (ms)
bool countUp = true;		// Sets if we are counting or not
bool counting = true;       // User controlled START and STOP
uint8_t buttonHold = 0;		
uint8_t clickTime = 0;
bool clicked = false;		// Has the button been pressed?
extern volatile bool user_typing;
extern volatile uint32_t typing_timeout;

 //Tracks last value sent over USB
uint32_t last_sent_count = 0xFFFFFFFF;

extern volatile bool cdc_connected;	// Checks if a USB terminal is connected or not


int main(void)
{
	atmel_start_init();
	usb_serial_begin();

	gpio_set_port_level(GPIO_PORTA, 1023, 1);	// Sends 1023 to the counter to light up all bars for a visual confirmation
	delay_ms(2000);								// Give it a couple seconds

	bool didSplash = false;						// Checking to see if the splash screen has been shown in USB

	while (1)
	{
		
		if (user_typing) {
			if (typing_timeout > 0) {
				typing_timeout--;
				} else {
				user_typing = false;
			}
		}
		
		
		// Checks if a terminal has been connected
		if (cdc_connected && !didSplash) {
			count = 1024;
			gpio_set_port_level(GPIO_PORTA, 1023, 1);
			serial_boot_message();
			count = 0;
			counting = false;   // We stop counting and set the counter to 0 on connect
			didSplash = true;	
		}
		// Checks disconnect on the terminal
		else if (!cdc_connected && didSplash) {
			didSplash = false;	
			counting = true;    // We keep counting from 0 once we disconnect
			count = 1024;		// Another visual check
			gpio_set_port_level(GPIO_PORTA, 1023, 1);
		}

		/* --- USB RX + echo + backspace support --- */
		if (cdc_connected) {
			char ch;
			while (usb_serial_read(&ch, 1) > 0)
			{
				if (ch == '\r' || ch == '\n') {
					usb_serial_write("\r\n", 2);
					if (usb_cmd_index > 0) {
						usb_cmd_buffer[usb_cmd_index] = '\0';
						handle_usb_command(usb_cmd_buffer);
						usb_cmd_index = 0;
					}
					continue;
				}
				if (ch == 0x08 || ch == 0x7F) { // Backspace
					if (usb_cmd_index > 0) {
						usb_cmd_index--;
						usb_serial_write("\b \b", 3); // flush immediately
					}
					continue;
				}
				if (usb_cmd_index < CMD_BUFFER_SIZE - 1) {
					usb_cmd_buffer[usb_cmd_index++] = ch;
					usb_serial_write(&ch, 1); // flush immediately
				}
			}
		}


		/* ------ Button-based manual control ------ */
		if(!gpio_get_pin_level(PA16) && buttonHold < 255){
			buttonHold++;
			for (volatile uint32_t i = 0; i < 600000; i++);
		}
		else if(gpio_get_pin_level(PA16) && buttonHold != 0){
			if(buttonHold < 20){
				counting ^= true;
				if(clicked){
					clickTime = 0;
					count = 1023;
					gpio_set_port_level(GPIO_PORTA, 1023, 1);
					counting = true;
					countUp = true;
				}
				else clicked = true;
			} else {
				countUp ^= true;
				counting = true;
			}
			buttonHold = 0;
		}
		else if (gpio_get_pin_level(PA16) && clicked){
			clickTime++;
			if(clickTime>5) clicked = false;
		}

		/* ------ COUNTER UPDATE (will run on COUNT under USB) ------ */
		if (counting && !user_typing) {
			if (countUp) count++;
			else count--;
			count %= 1024;
		}

		/* ------ LED Update ------ */
		gpio_set_port_level(GPIO_PORTA, 1023, 0);
		gpio_set_port_level(GPIO_PORTA, count & 1023, 1);

		/* ------ USB Streaming Output ONLY WHEN VALUE CHANGES ------ */
		if (cdc_connected && didSplash) {
			if (count != last_sent_count) {
				char msg[32];
				sprintf(msg, "COUNT: %lu\r\n", (unsigned long)count);
				usb_serial_write(msg, strlen(msg));
				last_sent_count = count;
			}
		}

		delay_ms(speed);
	}
}
