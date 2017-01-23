/*
 * wificontroller.c
 *
 *  Created on: Jan 21, 2017
 *      Author: francispapineau
 */

//! Includes
#include "wificontroller.hpp"
#include <stdlib.h>
#include <stdbool.h>
#include <Energia.h>
#include "configs.hpp"

///
/// Private
///

/// The Wifi client object
WiFiClient wifi_client;

/// The internal interface object
static wifi_interface_t wifi_iface;

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
	wifi_interface_t* iface = \
			(wifi_interface_t*) interface;

	// Deactivate
	iface->activated = false;
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: Deactivated");
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
	wifi_interface_t* iface = \
			(wifi_interface_t*) interface;

	// Deactivate
	iface->activated = false;
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: Activated");
	return ERROR_OK;
}

/**
 * @brief The Connect method for connecting to the wifi AP
 *
 * This method connects this node to a wifi AP based on the
 * SSID past and the past password. We should return true or false
 * base don the success of the connection.
 *
 * @return status	The status
 */
static error_t connect(){

	/*
	 * Start the wifi stack with the configuration definitions
	 */
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	/*
	 * Print out the connection sequence
	 */
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: Connecting to: " + String(WIFI_SSID));
	while(WiFi.status() != WL_CONNECTED){
		PRINT('.');
		digitalWrite(GREEN_LED, HIGH);
		delay(DEFAULT_DELAY);
		digitalWrite(WIFI_STATUS_LED, LOW);
		delay(DEFAULT_DELAY);
	}PRINTLN();

	/*
	 * Print out the ip.
	 */
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: Getting valid local ip.");
	WiFi.config(IPAddress(WIFI_IP));
	while(WiFi.localIP() == INADDR_NONE){
		PRINT('.');
		digitalWrite(WIFI_STATUS_LED, HIGH);
		delay(DEFAULT_DELAY);
		digitalWrite(WIFI_STATUS_LED, LOW);
		delay(DEFAULT_DELAY);
	}PRINTLN();

	/**
	 * Set DNS
	 */
#ifdef WIFI_LOCAL_DNS1 && WIFI_LOCAL_DNS2
	WiFi.setDNS(
				IPAddress(WIFI_LOCAL_DNS1),
				IPAddress(WIFI_LOCAL_DNS2)
				);
#endif

	/*
	 * We have an ip obtained
	 */
	IPAddress addr =  WiFi.localIP();
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: IP Address obtained: " + \
			String(addr[0]) + "." + \
			String(addr[1]) + "." + \
			String(addr[2]) + "." + \
			String(addr[3]));

	/**
	 * Turn on the indicator light
	 */
	digitalWrite(WIFI_STATUS_LED, HIGH);

	/*
	 * Return ok.
	 */
	return ERROR_OK;

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
	 * Disconnect from the access point.
	 */
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: Disconnecting from AP.");
	WiFi.disconnect();

	/**
	 * Turn off the indicator light
	 */
	digitalWrite(WIFI_STATUS_LED, LOW);
	return ERROR_OK;
}

/**
 * @brief Gets the internal status of the driver.
 *
 * @return status	The status
 */
static error_t status(){
	return WiFi.status() == WL_CONNECTED ? ERROR_OK : ERROR_TIMEOUT;
}

/**
 * @brief Prints out the info of the wifi stack.
 */
static error_t print_status(){

	/*
	 * Print Wifi Information
	 */
	IPAddress addr =  WiFi.localIP();
	String ip_address = String(addr[0]) + "." + \
				String(addr[1]) + "." + \
				String(addr[2]) + "." + \
				String(addr[3]);

	OS_PRINTLN(LEVEL_INFO, "[WIFI]: SSID: " + String(WiFi.SSID()));
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: IP: " + ip_address);
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: RSSI: " + String(WiFi.RSSI()));
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: Driver: " + String(WiFi.driverVersion()));
	OS_PRINTLN(LEVEL_INFO, "[WIFI]: Firmware: " + String(WiFi.firmwareVersion()));
	return ERROR_OK;
}

/**
 * @brief Initialize the interface.
 * Initializes the interface object.
 *
 * @return The interface pointer.
 */
static void* init(){

	// Activate the interface
	activate(&wifi_iface);
	return &wifi_iface;
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
static error_t ioctl(wifi_ioctl_t ioctl, void* arg){

	// Switch on the ioctl call
	switch(ioctl){
		/*
		 * Activate the interface
		 */
		case WIFI_IOCTL_ACTIVATE:
			return activate(arg);

		/*
		 * Deactivate the interface
		 */
		case WIFI_IOCTL_DEACTIVATE:
			return deactivate(arg);

		/*
		 * Connect the interface
		 */
		case WIFI_IOCTL_CONNECT:
			return connect();

		/*
		 * Disconnect the interface
		 */
		case WIFI_IOCTL_DISCONNECT:
			return disconnect();
		/*
		 * Get the status
		 */
		case WIFI_IOCTL_STATUS:
			return status();

		/*
		 * Get the status
		 */
		case WIFI_IOCTL_PRINT_STATUS:
			return print_status();

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
wifi_interface_t* get_WifiIface(){

	// Register the functions and return the object
	wifi_iface._init 		= init;
	wifi_iface._deinit 		= deinit;
	wifi_iface._ioctl 		= ioctl;
	wifi_iface.client		= &wifi_client;

	OS_PRINTLN(LEVEL_INFO, "[WIFI] Got the WIFI handle.");
	return &wifi_iface;
}
