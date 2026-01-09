/*
File:			main.c
Author:			Miller DeMark
Description:	Binary counter for ATSAMD21E15B with USB commands and debug LED sequence
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

// Counter variables
uint32_t count = 1023;		// Initial count
uint32_t speed = 200;		// Counter speed (ms)
bool countUp = true;
bool counting = true;
uint8_t buttonHold = 0;
uint8_t clickTime = 0;
bool clicked = false;

// DEBUG sequence variables
extern bool debug_mode;
extern uint8_t debug_step;
extern uint32_t debug_timer;

// Tracks last value sent over USB
uint32_t last_sent_count = 0xFFFFFFFF;

extern volatile bool user_typing;
extern volatile uint32_t typing_timeout;
extern volatile bool cdc_connected;

// LED patterns for DEBUG (matches 10-bit PORTA LEDs)
const uint16_t debug_leds[] = {
	1023, // all LEDs ON
	0x001, 0x002, 0x004, 0x008,
	0x010, 0x020, 0x040, 0x080,
	0x100, 0x200, 0x400
};
#define DEBUG_STEPS (sizeof(debug_leds)/sizeof(debug_leds[0]))

int main(void)
{
	atmel_start_init();
	usb_serial_begin();

	// Initial LED splash
	gpio_set_port_level(GPIO_PORTA, 1023, 1);
	delay_ms(2000);

	bool didSplash = false; // USB splash screen flag

	while (1)
	{
		// Handle typing timeout
		if (user_typing) {
			if (typing_timeout > 0) typing_timeout--;
			else user_typing = false;
		}

		// USB connection detection
		if (cdc_connected && !didSplash) {
			count = 1024;
			gpio_set_port_level(GPIO_PORTA, 1023, 1);
			serial_boot_message();
			count = 0;
			counting = false;
			didSplash = true;
		}
		else if (!cdc_connected && didSplash) {
			didSplash = false;
			counting = true;
			count = 1024;
			gpio_set_port_level(GPIO_PORTA, 1023, 1);
		}

		// USB RX + echo + backspace
		if (cdc_connected) {
			char ch;
			while (usb_serial_read(&ch, 1) > 0) {
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
						usb_serial_write("\b \b", 3);
					}
					continue;
				}
				if (usb_cmd_index < CMD_BUFFER_SIZE - 1) {
					usb_cmd_buffer[usb_cmd_index++] = ch;
					usb_serial_write(&ch, 1);
				}
			}
		}

		// ---- DEBUG LED sequence ----
		if (debug_mode) {
			debug_timer += speed; // accumulate time

			if (debug_step < DEBUG_STEPS) {
				count = debug_leds[debug_step];
				if (debug_timer >= 500) { // 500ms per step
					debug_step++;
					debug_timer = 0;
				}
				} else {
				// DEBUG done
				debug_mode = false;
				counting = true; // resume normal counter
				count = 0;       // clear LEDs
			}
		}

		// ---- Normal counter update (only if not in debug) ----
		if (!debug_mode && counting && !user_typing) {
			if (countUp) count++;
			else count--;
			count %= 1024;
		}

		// ---- LED Update ----
		gpio_set_port_level(GPIO_PORTA, 1023, 0);
		gpio_set_port_level(GPIO_PORTA, count & 1023, 1);

		// ---- USB streaming output ----
		if (cdc_connected && didSplash) {
			if (count != last_sent_count) {
				char msg[32];
				sprintf(msg, "COUNT: %lu\r\n", (unsigned long)count);
				usb_serial_write(msg, strlen(msg));
				last_sent_count = count;
			}
		}

		// ---- Button control ----
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
				} else clicked = true;
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

		delay_ms(speed);
	}
}
