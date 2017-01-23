/*
 * ircontroller.c
 *
 *  Created on: Jan 21, 2017
 *      Author: francispapineau
 */

//! Includes
#include "ircontroller.hpp"

#include <stdlib.h>
#include <stdbool.h>
#include <Energia.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <avr/pgmspace.h>

#include "configs.hpp"

///
/// Private
///

/// The buffers for the commands
static prog_uint16_t POWER_OFF[] 			PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		552, 552, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t POWER_ON[]  			PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552,
		552, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 552,
		552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t BRIGHTNESS_UP[] 		PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552,
		552, 552, 552, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 1683,
		552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t BRIGHTNESS_DOWN[] 		PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552,
		552, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 552,
		552, 552, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t FLASH[] 				PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552,
		552, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 1683, 552, 552, 552, 552, 552, 1683, 552, 552,
		552, 552, 552, 1683, 552, 1683, 552, 552, 552, 1683, 552, 39996, 8967, 2261, 552,
};
static prog_uint16_t STROBE[] 				PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 1683, 552, 1683,
		552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};
static prog_uint16_t FADE[] 				PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		1683, 552, 552, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 552, 552, 1683,
		552, 552, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};
static prog_uint16_t SMOOTH[] 				PROGMEM  = {
		8967, 4523, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		552, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 1683, 552,
		552, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t COLOR_WHITE[] 			PROGMEM  = {
		8967, 4523, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683,
		552, 552, 552, 1683, 552, 552, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683,
		552, 552, 552, 1683, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};
static prog_uint16_t COLOR_RED[] 			PROGMEM  = {
		8967, 4523, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683,
		552, 552, 552, 552, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683,
		552, 1683, 552, 552, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};
static prog_uint16_t COLOR_RED_ORANGE[] 	PROGMEM  = {
		8967, 4523, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552,
		1683, 552, 1683, 552, 1683, 552, 552, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		552, 552, 552, 552, 1683, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};
static prog_uint16_t COLOR_ORANGE[] 		PROGMEM  = {
		8993, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552,
		552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 552, 552, 1683, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552, 1683, 552, 40101, 8993, 2261, 552
};
static prog_uint16_t COLOR_ORANGE_YELLOW[] 	PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		1683, 552, 552, 552, 1683,	552, 552, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 552, 552, 1683,
		552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t COLOR_YELLOW[] 		PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		552, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 1683, 552,
		552, 552, 552, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
} ;
static prog_uint16_t COLOR_GREEN[] 			PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552,
		1683, 552, 552, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683,
		552, 552, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t COLOR_LIGHT_GREEN[] 	PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		1683, 552, 552, 552, 552, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 552, 552, 1683,
		552, 1683, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t COLOR_GREEN_BLUE[] 	PROGMEM  = {
		8967, 4523, 552, 552, 552, 552, 552, 552 ,552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		552, 552, 1683, 552, 1683,	552, 552, 552, 552, 552, 1683, 552, 552, 552, 1683, 552, 1683,	552,
		552, 552, 552, 552, 1683, 552, 1683, 552, 552, 552, 1683,	552, 39996, 8967, 2261, 552
};
static prog_uint16_t COLOR_LIGHT_BLUE[] 	PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 552, 552,
		552, 552, 552, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};
static prog_uint16_t COLOR_MEDIUM_BLUE[] 	PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		552, 552, 1683, 552, 552, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 1683, 552, 552,
		552, 1683, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t COLOR_BLUE[] 			PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683,
		552, 552, 552, 552, 552, 552, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683,
		552, 1683, 552, 1683, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};
static prog_uint16_t COLOR_DARK_BLUE[] 		PROGMEM  = {
		8967, 4523, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		1683, 552, 1683, 552, 552, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 552, 552, 552,
		552, 1683, 552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};
static prog_uint16_t COLOR_DARK_PURPLE[] 	PROGMEM  = {
		8967, 4523, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		552, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 1683, 552, 552,
		552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};
static prog_uint16_t COLOR_MEDIUM_PURPLE[] PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 1683, 552, 552, 552, 552,
		552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 39996, 8967, 2261, 552
};
static prog_uint16_t COLOR_LIGHT_PURPLE[]	PROGMEM  = {
		8967, 4497, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552,
		1683, 552, 1683, 552, 1683, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552, 552,
		552, 552, 552, 1683, 552, 1683, 552, 1683, 552, 1683, 552, 40022, 8967, 2261, 552
};

/// The database
static ir_command_entry_t ir_commands_db[] = {
		{	IR_COMMAND_OFF, 				POWER_OFF, 				sizeof(POWER_OFF)			},
		{	IR_COMMAND_ON, 					POWER_ON, 				sizeof(POWER_ON)			},
		{	IR_COMMAND_BRIGHTNESS_UP, 		BRIGHTNESS_UP, 			sizeof(BRIGHTNESS_UP)		},
		{	IR_COMMAND_BRIGHTNESS_DOWN, 	BRIGHTNESS_DOWN, 		sizeof(BRIGHTNESS_DOWN)		},
		{	IR_COMMAND_FLASH, 				FLASH,					sizeof(FLASH)				},
		{	IR_COMMAND_STROBE,				STROBE,					sizeof(STROBE)				},
		{	IR_COMMAND_FADE,				FADE,					sizeof(FADE)				},
		{	IR_COMMAND_SMOOTH,				SMOOTH,					sizeof(SMOOTH)				},
		{	IR_COMMAND_WHITE,				COLOR_WHITE,			sizeof(COLOR_WHITE)			},
		{	IR_COMMAND_RED,					COLOR_RED,				sizeof(COLOR_RED)			},
		{	IR_COMMAND_RED_ORANGE,			COLOR_RED_ORANGE,		sizeof(COLOR_RED_ORANGE)	},
		{	IR_COMMAND_ORANGE,				COLOR_ORANGE,			sizeof(COLOR_ORANGE)		},
		{	IR_COMMAND_ORANGE_YELLOW,		COLOR_ORANGE_YELLOW,	sizeof(COLOR_ORANGE_YELLOW)	},
		{	IR_COMMAND_YELLOW,				COLOR_YELLOW, 			sizeof(COLOR_YELLOW)		},
		{	IR_COMMAND_GREEN,				COLOR_GREEN, 			sizeof(COLOR_GREEN)			},
		{	IR_COMMAND_LIGHT_GREEN,			COLOR_LIGHT_GREEN, 		sizeof(COLOR_LIGHT_GREEN)	},
		{	IR_COMMAND_GREEN_BLUE,			COLOR_GREEN_BLUE, 		sizeof(COLOR_GREEN_BLUE)	},
		{	IR_COMMAND_LIGHT_BLUE,			COLOR_LIGHT_BLUE, 		sizeof(COLOR_LIGHT_BLUE)	},
		{	IR_COMMAND_MEDIUM_BLUE,			COLOR_MEDIUM_BLUE, 		sizeof(COLOR_MEDIUM_BLUE)	},
		{	IR_COMMAND_BLUE,				COLOR_BLUE,				sizeof(COLOR_BLUE)			},
		{	IR_COMMAND_DARK_BLUE,			COLOR_DARK_BLUE, 		sizeof(COLOR_DARK_BLUE)		},
		{ 	IR_COMMAND_DARK_PURPLE,			COLOR_DARK_PURPLE, 		sizeof(COLOR_DARK_PURPLE)	},
		{	IR_COMMAND_MEDIUM_PURPLE, 		COLOR_MEDIUM_PURPLE, 	sizeof(COLOR_MEDIUM_PURPLE)	},
		{	IR_COMMAND_LIGHT_PURPLE,		COLOR_LIGHT_PURPLE, 	sizeof(COLOR_LIGHT_PURPLE)	},
		{	IR_COMMAND_NONE,				IR_DEFAULT_END_DB, 		IR_DEFAULT_END_DB			},
};

/// Define the internal interface objects
/// The IR interface library handle
static IRsend iface;

/// The internal interface object
static ir_interface_t ir_iface;

/**
 * @brief Deactivates the interface.
 *
 * @param interface		The interface pointer
 * @return error_t
 */
static error_t deactivate(void* interface){

	// Null check
	if(!interface) {
		OS_PRINTLN(LEVEL_ERROR, "NULL Pointer.");
		return ERROR_NULL_PTR;
	}

	// Cast to a usable ptr.
	ir_interface_t* iface = \
			(ir_interface_t*) interface;

	// Deactivate
	iface->activated = false;
	OS_PRINTLN(LEVEL_INFO, "[IR]: Deactivated");
	return ERROR_OK;
}

/**
 * @brief Activates the interface
 *
 * @param interface		The interface pointer
 * @return error_t
 */
static error_t activate(void* interface){

	// Null check
	if(!interface) {
		OS_PRINTLN(LEVEL_ERROR, "NULL Pointer.");
		return ERROR_NULL_PTR;
	}

	// Cast to a usable ptr.
	ir_interface_t* iface = \
			(ir_interface_t*) interface;

	// Deactivate
	iface->activated = false;
	OS_PRINTLN(LEVEL_INFO, "[IR]: Activated");
	return ERROR_OK;
}

/**
 * @brief Checks to see if a command is valid.
 *
 * This function will cycle through the commands database and
 * check whether the command is valid or not.
 *
 * @param command		The command to check
 * @param entry			The pointer to the database
 * @return error_t
 */
static error_t check_command(ir_command_t command, ir_command_entry_t** entry){

	// Temporary
	ir_command_t index;

	// Null check
	if(!entry) {
		OS_PRINTLN(LEVEL_ERROR, "NULL Pointer.");
		return ERROR_NULL_PTR;
	}

	// Check to see if the command is there
	for(index = IR_COMMAND_ON; index < IR_COMMAND_NONE; index++){

		// Check the command
		if(index == command){

			// Set the entry pointer
			*entry = ir_commands_db[index];
			OS_PRINTLN(LEVEL_INFO,
					("[IR] Got entry: " + String(index)));
			return ERROR_OK;
		}
	}

	// Fail
	OS_PRINTLN(LEVEL_DEBUG, "No command match")
	return ERROR_NO_MATCH;
}

/**
 * @brief Send out a command
 *
 * @param command		The command to send
 * @return error_t
 */
static error_t send(ir_command_t command){

	// Temporary
	unsigned int* buffer;

	// Default (Set to 0)
	ir_command_entry_t* entry = \
			ir_commands_db[IR_COMMAND_NONE];

	// Check to see if its up
	if(!ir_iface.activated){

		// Not activated
		OS_PRINTLN(LEVEL_ERROR, "[IR] Device not activated.");
		return ERROR_NOT_ACTIVATED;

	}else{
		// Check to see if valid
		if(ERROR_OK != \
				check_command(command, &entry)){

			// Not a real command
			OS_PRINTLN(LEVEL_ERROR, "[IR] Not a valid entry: " + String(command));
			return ERROR_COMMAND_NOT_VALID;

		}
		else {
			// Convert the PROGMEM item into a byte array
			buffer = malloc(entry->size);

			// Copy over the command
			for(int i = 0; i < entry->size; i ++){
				buffer[i] = pgm_read_word_near(entry->command_array + i);
			}

			// Send the command
			for(int retry = 0; retry < IR_DEFAULT_RETRY_COUNT; retry++){

				// Send the data
				iface.sendRaw(
						(unsigned int*) entry->command_array,
						entry->size,
						IR_DEFAULT_HZ);
				OS_PRINTLN(LEVEL_INFO, "[IR] Sent: " + String(entry->command));
			}

			// We are good
			return ERROR_OK;
		}
	}
}

/**
 * @brief Initialize the interface.
 * Initializes the interface object.
 *
 * @return The interface pointer.
 */
static void* init(){

	// Activate the interface
	activate(&ir_iface);
	return &ir_iface;
}

/**
 * @brief Destroys the passed in interface.
 *
 * @param interface
 */
static void deinit(void* interface){

	// Deactivate the interface
	deactivate(interface);
	return;
}

/**
 * @brief The public ioctl system call.
 * This is the main entry point for the interface
 *
 * @param ioctl			The ioctl to call
 * @return error_t
 */
static error_t ioctl(ir_ioctl_t ioctl, void* arg){

	// Switch on the ioctl call
	switch(ioctl){
		/*
		 * Activate the interface
		 */
		case IR_IOCTL_ACTIVATE:
			return activate(arg);

		/*
		 * Deactivate the interface
		 */
		case IR_IOCTL_DEACTIVATE:
			return deactivate(arg);
		/*
		 * Send the command
		 */
		case IR_IOCTL_SEND:
			return send((ir_command_t) arg);

		/*
		 * Default case
		 */
		default:
			return ERROR_NOT_SUPPORTED_IOCTL;
	}
}

///
/// Public
///

/**
 * @brief The getter for the interface
 *
 * @return ir_interface_t
 */
ir_interface_t* get_IrIface(){

	// Register the functions and return the object
	ir_iface._init 		= init;
	ir_iface._deinit 	= deinit;
	ir_iface._ioctl 	= ioctl;

	OS_PRINTLN(LEVEL_INFO, "[IR] Got the IR handle.");
	return &ir_iface;
}
