/*
 * ircontroller.h
 *
 *  Created on: Jan 21, 2017
 *      Author: francispapineau
 */

#ifndef SMARTIRREMOTE_IRCONTROLLER_H_
#define SMARTIRREMOTE_IRCONTROLLER_H_

//! Includes
#include <stdlib.h>
#include "error.hpp"

/**
 * This is the IR controller interface that will be used
 * to send and receive command from and to the IR blaster.
 */

/// Define the number of times to send out the IR command
#define IR_DEFAULT_RETRY_COUNT		(3)

/// Defines the end of the database
#define IR_DEFAULT_END_DB			(0)

/// Defines the default HZ
#define IR_DEFAULT_HZ				(37)

/**
 * This is the various available commands for the
 * IR sender.
 */
typedef enum {

	/*
	 * Light operation commands
	 */

	IR_COMMAND_ON,             		//!< IR_COMMAND_ON
	IR_COMMAND_OFF,            		//!< IR_COMMAND_OFF
	IR_COMMAND_BRIGHTNESS_UP,  		//!< IR_COMMAND_BRIGHTNESS_UP
	IR_COMMAND_BRIGHTNESS_DOWN,		//!< IR_COMMAND_BRIGHTNESS_DOWN
	IR_COMMAND_FLASH,          		//!< IR_COMMAND_FLASH
	IR_COMMAND_STROBE,         		//!< IR_COMMAND_STROBE
	IR_COMMAND_FADE,           		//!< IR_COMMAND_FADE
	IR_COMMAND_SMOOTH,         		//!< IR_COMMAND_SMOOTH

	/*
	 * Light colour operation
	 */

	IR_COMMAND_WHITE,          		//!< IR_COMMAND_WHITE
	IR_COMMAND_RED,            		//!< IR_COMMAND_RED
	IR_COMMAND_RED_ORANGE,     		//!< IR_COMMAND_RED_ORANGE
	IR_COMMAND_ORANGE,         		//!< IR_COMMAND_ORANGE
	IR_COMMAND_ORANGE_YELLOW,  		//!< IR_COMMAND_ORANGE_YELLOW
	IR_COMMAND_YELLOW,         		//!< IR_COMMAND_YELLOW
	IR_COMMAND_GREEN,          		//!< IR_COMMAND_GREEN
	IR_COMMAND_LIGHT_GREEN,    		//!< IR_COMMAND_LIGHT_GREEN
	IR_COMMAND_GREEN_BLUE,     		//!< IR_COMMAND_GREEN_BLUE
	IR_COMMAND_LIGHT_BLUE,     		//!< IR_COMMAND_LIGHT_BLUE
	IR_COMMAND_MEDIUM_BLUE,    		//!< IR_COMMAND_MEDIUM_BLUE
	IR_COMMAND_BLUE,           		//!< IR_COMMAND_BLUE
	IR_COMMAND_DARK_BLUE,      		//!< IR_COMMAND_DARK_BLUE
	IR_COMMAND_DARK_PURPLE,    		//!< IR_COMMAND_DARK_PURPLE
	IR_COMMAND_MEDIUM_PURPLE,  		//!< IR_COMMAND_MEDIUM_PURPLE
	IR_COMMAND_LIGHT_PURPLE,    	//!< IR_COMMAND_LIGHT_PURPLE
	IR_COMMAND_NONE,				//!< IR_COMMAND_NONE
}ir_command_t;

/**
 * The IOCTLs to call
 */
typedef enum {
	IR_IOCTL_ACTIVATE,  			//!< IR_IOCTL_ACTIVATE
	IR_IOCTL_DEACTIVATE,			//!< IR_IOCTL_DEACTIVATE
	IR_IOCTL_SEND       			//!< IR_IOCTL_SEND
}ir_ioctl_t;

/**
 * @brief Initialize the interface.
 * Initializes the interface object.
 *
 * @return The interface pointer.
 */
typedef void (*init_t)();

/**
 * @brief Destroys the passed in interface.
 *
 * @param interface
 */
typedef void (*deinit_t)(void* interface);

/**
 * @brief The public ioctl system call.
 *
 * @param ioctl		The ioctl to call
 * @param arg		The arguments
 */
typedef error_t (*ioctl_t)(ir_ioctl_t ioctl, void* arg);

/**
 * This is the IR interface.
 * It will register all the functions necessary to
 * send and receive the IR commands.
 *
 * It will also register a bunch of interface type
 * getters and setters for the interface.
 */
typedef struct {

	// The interface attributes.
	bool 				activated;

	// The registered apis.
	init_t 				_init;
	deinit_t 			_deinit;
	ioctl_t				_ioctl;
}ir_interface_t;

/**
 * Defines the command entry with the commands database.
 */
typedef struct {

	// The command
	ir_command_t 		command;

	// The array
	prog_uint16_t* 		command_array;

	// The size
	int					size;
}ir_command_entry_t;

/**
 * @brief The getter for the interface
 *
 * @return
 */
ir_interface_t* get_IrIface();


#endif /* SMARTIRREMOTE_IRCONTROLLER_H_ */
