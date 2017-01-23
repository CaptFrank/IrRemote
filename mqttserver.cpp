/*
 * mqttserver.c
 *
 *  Created on: Jan 21, 2017
 *      Author: francispapineau
 */

//! Includes
#include "mqttserver.hpp"
#include "wificontroller.hpp"
#include "ircontroller.hpp"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <Energia.h>
#include "configs.hpp"

///
/// Private
///

/**
 * @brief Get the wifi interface
 */
extern WiFiClient wifi_client;

/**
 * @brief Get the ir controller
 */
extern ir_interface_t ir_iface;

/**
 * @brief The mqtt interface
 */
static mqtt_interface_t mqtt_iface;

/**
 * @brief Define the callback
 */
void callback(char*, uint8_t*, unsigned int);

/**
 * @brief Define the pub sub client object
 */
static PubSubClient mqtt_client(
	(char*)MQTT_BROKER_ADDR,
	MQTT_BROKER_PORT,
	callback,
	wifi_client
);

/**
 * @brief Define the static message to send
 */
static mqtt_message_t msg;

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
	mqtt_interface_t* iface = \
			(mqtt_interface_t*) interface;

	// Deactivate
	iface->activated = false;
	OS_PRINTLN(LEVEL_INFO, "[MQTT]: Deactivated");
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
	mqtt_interface_t* iface = \
			(mqtt_interface_t*) interface;

	// Deactivate
	iface->activated = false;
	OS_PRINTLN(LEVEL_INFO, "[MQTT]: Activated");
	return ERROR_OK;
}

/**
 * @brief Subscribes to a topic with registered message type and handler.
 *
 * @param topic			The topic to subscribe
 * @return status		The status
 */
static error_t subscribe(mqtt_topic_t topic){

	/*
	 * Subscribe to a topic
	 */
#ifdef DEBUG_ON
	OS_PRINTLN(LEVEL_DEBUG, "[MQTT]: Subscribing to: " + String(topic));
#endif
	if(mqtt_client.subscribe(topic)){

#ifdef DEBUG_ON
		OS_PRINTLN(LEVEL_DEBUG, "[MQTT]: Subscribing complete!");
#endif
		return ERROR_OK;
	}
	else {
		OS_PRINTLN(LEVEL_ERROR, "[MQTT]: Subscribe issue!");
		return ERROR_SUBSCRIBING;
	}

	return ERROR_TIMEOUT;
}

/**
 * @brief Publishes a message to a specified topic.
 *
 * @param mqtt_message 	An mqtt message type
 */
static error_t publish(mqtt_message_t* msg){

	/**
	 * Flush the interface fisrt
	 */
	wifi_client.flush();

	/*
	 * Publish a message to the broker
	 */
	return \
		mqtt_client.publish((char*)msg->topic, (uint8_t*)msg->payload, msg->length) ? \
				ERROR_OK : ERROR_PUBLISHING;
}


/**
 * @brief The Connect method for connecting to the mqtt AP
 *
 * This method connects this node to an mqtt server.
 *
 * @return status		The status
 */
static error_t connect(){

	/*
	 * Connect to the broker if not connected already
	 */
	if(!mqtt_client.connected()){
#ifdef DEBUG_ON
		OS_PRINTLN(LEVEL_DEBUG, "[MQTT]: Not connected...");
		OS_PRINTLN(LEVEL_DEBUG, "[MQTT]: Connecting to broker.");
#endif

		if(!mqtt_client.connect((char*)MQTT_SYSTEM_ID)){
			OS_PRINTLN(LEVEL_ERROR, "[MQTT]: Connection failed!");
			digitalWrite(MQTT_STATUS_LED, LOW);
			return ERROR_CONNECTION;
		}
		else {
#ifdef DEBUG_ON
			OS_PRINTLN(LEVEL_DEBUG, "[MQTT]: Connection success!");
			OS_PRINTLN(LEVEL_DEBUG, "[MQTT]: Connected to: " \
					+ String(MQTT_BROKER_ADDR) \
					+ ":" \
					+ String(MQTT_BROKER_PORT));

			OS_PRINTLN(LEVEL_DEBUG, "[MQTT]: Subscribing to: " + String(MQTT_SUB_TOPIC));
#endif

			digitalWrite(MQTT_STATUS_LED, HIGH);
			if(subscribe((char*)MQTT_SUB_TOPIC) == ERROR_OK){

#ifdef DEBUG_ON
				OS_PRINTLN(LEVEL_DEBUG, "[MQTT]: Subscribing complete!");
#endif
				return ERROR_OK;
			}
			else {
				OS_PRINTLN(LEVEL_ERROR, "[MQTT]: Subscribe issue!");
				digitalWrite(MQTT_STATUS_LED, LOW);
				return ERROR_CONNECTION;
			}
		}
	}
	return STATUS_ERR_TIMEOUT;
}

/**
 * @brief The Disconnection mechansim for the wifi conenction.
 *
 * This method disconnects the node from the AP and returns
 * a bool representing the success of the disconnection.
 *
 * @return status	The status
 */
static error_t disconnect(){

	/*
	 * Disconnect from MQTT broker
	 */
	mqtt_client.disconnect();
	return ERROR_OK;
}

/**
 * @brief Gets the internal status of the driver.
 *
 * @return status		The status
 */
static error_t status(){

	/*
	 * Return connection status
	 */
	return mqtt_client.connected() ? ERROR_OK : ERROR_CONNECTION;
}

/**
 * @brief Creates a message to send via MQTT
 *
 * @param topic			The topic
 * @param payload		The payload
 * @param length		The lengths
 * @return msg			The message
 */
static mqtt_message_t* packetize(
		mqtt_topic_t topic, char* payload, unsigned int length){

	/*
	 * Create the message to send
	 */
	msg.topic 	= topic;
	msg.payload = payload;
	msg.length 	= length;

	/*
	 * Return a reference
	 */
	return &msg;
}

/**
 * @brief Define the callback processor
 */
static void callback(char* topic, uint8_t* buffer, unsigned int size){

	// Response pointer
	mqtt_message_t* response;
	char* response_buffer = malloc(strlen(MQTT_ACK_RESPONSE));

	/*
	 * Check to see if the packet is too big.
	 * If so return with no processing done.
	 */
	if(size > MQTT_MAX_PACKET_SIZE){
		OS_PRINTLN(LEVEL_ERROR, "[MQTT]: Payload is to big to process.");
		return;
	}
	OS_PRINTLN(LEVEL_INFO, "[MQTT]: Got a new Message from: " + String(topic));

	/*
	 *	Figure out the message
	 */
	char* message = (char*) malloc(size - 2);
	strncpy(message, (char*) buffer, size - 2);
	OS_PRINTLN(LEVEL_INFO, "[MQTT]: Message received: " + String(message));

	/*
	 * Execute the command
	 */
	if(ERROR_OK != ir_iface._ioctl(IR_IOCTL_SEND, message)){
		OS_PRINTLN(LEVEL_ERROR, "[MQTT]: Issue sending the IR code.");
	}
	else {

		// Create a response.
		sprintf(response_buffer, MQTT_ACK_RESPONSE, message);
		response = packetize(
				MQTT_PUB_TOPIC,
				response_buffer,
				strlen(response_buffer));

		// Send it
		if(ERROR_OK != publish(response)){
			OS_PRINTLN(LEVEL_ERROR, "[MQTT]: Issue sending the response.");
		}
	}
	/*
	 * Free the message
	 */
	free(message);
	free(response_buffer);
	return;
}

/**
 * @brief Initialize the interface.
 * Initializes the interface object.
 *
 * @return The interface pointer.
 */
static void* init(){

	// Activate the interface
	activate(&mqtt_iface);
	return &mqtt_iface;
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
static error_t ioctl(mqtt_ioctl_t ioctl, void* arg){

	// Switch on the ioctl call
	switch(ioctl){
		/*
		 * Activate the interface
		 */
		case MQTT_IOCTL_ACTIVATE:
			return activate(arg);

		/*
		 * Deactivate the interface
		 */
		case MQTT_IOCTL_DEACTIVATE:
			return deactivate(arg);

		/*
		 * Connect the interface
		 */
		case MQTT_IOCTL_CONNECT:
			return connect();

		/*
		 * Disconnect the interface
		 */
		case MQTT_IOCTL_DISCONNECT:
			return disconnect();

		/*
		 * Subscribe to a topic
		 */
		case MQTT_IOCTL_SUB:
			return subscribe((mqtt_topic_t) arg);

		/*
		 * Publish a message
		 */
		case MQTT_IOCTL_PUB:
			return publish((mqtt_message_t*) arg);

		/*
		 * Get the status
		 */
		case MQTT_IOCTL_STATUS:
			return status();

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
 * @return
 */
mqtt_interface_t* get_MqttIface(){

	// Register the functions and return the object
	mqtt_iface._init 		= init;
	mqtt_iface._deinit 		= deinit;
	mqtt_iface._ioctl 		= ioctl;
	mqtt_iface.client		= &mqtt_client;

	OS_PRINTLN(LEVEL_INFO, "[MQTT] Got the MQTT handle.");
	return &mqtt_iface;
}
/**
 * @brief The server method.
 *
 * @return
 */
error_t serve_MqttIface(){

	/**
	 * Flush the wifi
	 */
	get_WifiIface()->client->flush();

	/*
	 * Run the MQTT stack
	 */
	return mqtt_client.poll() == true ? ERROR_OK : ERROR_CONNECTION;
}
