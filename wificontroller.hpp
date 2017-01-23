/*
 * wificontroller.h
 *
 *  Created on: Jan 21, 2017
 *      Author: francispapineau
 */

#ifndef SMARTIRREMOTE_WIFICONTROLLER_H_
#define SMARTIRREMOTE_WIFICONTROLLER_H_

//! Includes
#include <stdlib.h>
#include <WiFi.h>

#include "error.hpp"


/**
 * The IOCTLs to call
 */
typedef enum {
	WIFI_IOCTL_ACTIVATE,
	WIFI_IOCTL_DEACTIVATE,
	WIFI_IOCTL_CONNECT,
	WIFI_IOCTL_DISCONNECT,
	WIFI_IOCTL_STATUS,
	WIFI_IOCTL_PRINT_STATUS
}wifi_ioctl_t;

/**
 * @brief Typedefs
 */
typedef char* mqtt_topic_t;

/**
 * This is the message type
 */
typedef struct {

	mqtt_topic_t 	topic;				//!< The topic
	char* 			payload;			//!< The payload in bytes
	unsigned int 	length;				//!< The payload length
}mqtt_message_t;

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
typedef error_t (*ioctl_t)(wifi_ioctl_t ioctl, void* arg);

/**
 * This is the WIFI interface.
 * It will register all the functions necessary to
 * send and receive the WIFI commands.
 *
 * It will also register a bunch of interface type
 * getters and setters for the interface.
 */
typedef struct {

	// The interface attributes.
	bool 				activated;

	// The wifi client object pointer
	WiFiClient*			client;

	// The registered apis.
	init_t 				_init;
	deinit_t 			_deinit;
	ioctl_t				_ioctl;
}wifi_interface_t;

/**
 * @brief The getter for the interface
 *
 * @return
 */
wifi_interface_t* get_WifiIface();


#endif /* SMARTIRREMOTE_WIFICONTROLLER_H_ */
