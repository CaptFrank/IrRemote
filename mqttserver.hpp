/*
 * mqttserver.h
 *
 *  Created on: Jan 21, 2017
 *      Author: francispapineau
 */

#ifndef SMARTIRREMOTE_MQTTSERVER_H_
#define SMARTIRREMOTE_MQTTSERVER_H_

//! Includes
#include <stdlib.h>
#include <PubSubClient.h>
#include "error.hpp"

/**
 * @brief Defines the end of callbacks
 */
#define DEFAULT_END_CALLBACK_LIST	(NULL)

/**
 * @brief Defines a good compare
 */
#define DEFAULT_SUCCESS_COMPARE		(0)

/**
 * The IOCTLs to call
 */
typedef enum {
	MQTT_IOCTL_ACTIVATE,  //!< MQTT_IOCTL_ACTIVATE
	MQTT_IOCTL_DEACTIVATE,//!< MQTT_IOCTL_DEACTIVATE
	MQTT_IOCTL_CONNECT,   //!< MQTT_IOCTL_CONNECT
	MQTT_IOCTL_DISCONNECT,//!< MQTT_IOCTL_DISCONNECT
	MQTT_IOCTL_SUB,       //!< MQTT_IOCTL_SUB
	MQTT_IOCTL_PUB,       //!< MQTT_IOCTL_PUB
	MQTT_IOCTL_STATUS     //!< MQTT_IOCTL_STATUS
}mqtt_ioctl_t;

/**
 * @brief Defines the callback type
 */
typedef error_t (*callback)(char* topic, uint8_t* buffer, int size);

/**
 * Defines the callback structure type
 */
typedef struct {

	// The channel to filter on.
	char* 		channel;

	// The callback to call
	callback 	_callback;

	// The next callback
	callback* 	_next;
}mqtt_callback_t;

/**
 * @brief Initialize the interface.
 * Initializes the interface object and registers the
 * callbacks needed.
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
typedef error_t (*ioctl_t)(mqtt_ioctl_t ioctl, void* arg);

/**
 * This is the MQTT interface.
 * It will register all the functions necessary to
 * send and receive the MQTT commands.
 *
 * It will also register a bunch of interface type
 * getters and setters for the interface.
 */
typedef struct {

	// The interface attributes.
	bool 				activated;

	// The pub/sub client ptr
	PubSubClient*		client;

	// The callback list
	mqtt_callback_t*	callbacks;

	// The registered apis.
	init_t 				_init;
	deinit_t 			_deinit;
	ioctl_t				_ioctl;
}mqtt_interface_t;

/**
 * @brief The getter for the interface
 *
 * @return
 */
mqtt_interface_t* get_MqttIface();

/**
 * @brief The server method.
 *
 * @return
 */
error_t serve_MqttIface();

/**
 * @brief Creates a message to send via MQTT
 *
 * @param topic			The topic
 * @param payload		The payload
 * @param length		The lengths
 * @return msg			The message
 */
mqtt_message_t* packetize(
		mqtt_topic_t topic, char* payload, unsigned int length);

#endif /* SMARTIRREMOTE_MQTTSERVER_H_ */
