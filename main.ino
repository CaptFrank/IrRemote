/*
 * main.c
 *
 *  Created on: Jan 21, 2017
 *      Author: francispapineau
 */

#include <Energia.h>

#include "error.hpp"
#include "configs.hpp"
#include "ircontroller.hpp"
#include "mqttserver.hpp"
#include "wificontroller.hpp"

/**
 * Global handles
 */
ir_interface_t* ir;
wifi_interface_t* wifi;
mqtt_interface_t* mqtt;

/**
 * Global error
 */
error_t system_error = ERROR_OK;

/**
 * Simple method to setup IR
 * @return
 */
static error_t setupIr(){

	// Get the IR handle
	ir = get_IrIface();

	// Valid?
	if(!ir){
		OS_PRINTLN(LEVEL_ERROR, "[OS] Issue setting up IR.")
		return ERROR_NULL_PTR;
	}
	else{

		// Init the IR handle
		if(ERROR_OK != ir->_init()){
			OS_PRINTLN(LEVEL_ERROR, "[OS] Issue init IR.")
			return ERROR_IR_SETUP_FAIL;
		}
		else{
			OS_PRINTLN(LEVEL_INFO, "[OS] Success init IR")
			return ERROR_OK;
		}
	}
}

/**
 * Simple method to setup WIFI
 * @return
 */
static error_t setupWifi(){

	// Get the WIFI handle
	wifi = get_WifiIface();

	// Valid?
	if(!wifi){
		OS_PRINTLN(LEVEL_ERROR, "[OS] Issue setting up WIFI.")
		return ERROR_NULL_PTR;
	}
	else{

		// Init the WIFI handle
		if(ERROR_OK != wifi->_init()){
			goto fail;
		}
		else{
			if(ERROR_OK != wifi->_ioctl(WIFI_IOCTL_CONNECT, NULL)){
				goto fail;
			}
			else {
				if(ERROR_OK != wifi->_ioctl(WIFI_IOCTL_PRINT_STATUS, NULL)){
					goto fail;
				}
				else {
					OS_PRINTLN(LEVEL_INFO, "[OS] Success init WIFI")
					return ERROR_OK;
				}
			}
		}
	}
	fail:
	OS_PRINTLN(LEVEL_ERROR, "[OS] Issue init WIFI.")
	return ERROR_IR_SETUP_FAIL;
}

/**
 * Simple method to setup MQTT
 * @return
 */
static error_t setupMqtt(){

	// Temp
	mqtt_message_t* message = \
			packetize(MQTT_PUB_TOPIC,
					MQTT_ACK_SYSTEM,
					strlen(MQTT_ACK_SYSTEM));

	// Get the MQTT handle
	mqtt = get_MqttIface();

	// Valid?
	if(!mqtt){
		OS_PRINTLN(LEVEL_ERROR, "[OS] Issue setting up MQTT.")
		return ERROR_NULL_PTR;
	}
	else{

		// Init the MQTT handle
		if(ERROR_OK != mqtt->_init()){
			goto fail;
		}
		else{
			if(ERROR_OK != mqtt->_ioctl(MQTT_IOCTL_CONNECT, NULL)){
				goto fail;
			}
			else {
				if(ERROR_OK != mqtt->_ioctl(MQTT_IOCTL_SUB, MQTT_SUB_TOPIC)){
					goto fail;
				}
				else {
					if(ERROR_OK != mqtt->_ioctl(MQTT_IOCTL_STATUS, NULL)){
						goto fail;
					}
					else {
						if(ERROR_OK != mqtt->_ioctl(MQTT_IOCTL_PUB, message)){
							goto fail;
						}
						else {
							OS_PRINTLN(LEVEL_INFO, "[OS] Success init MQTT")
							return ERROR_OK;
						}
					}
				}
			}
		}
	}
	fail:
	OS_PRINTLN(LEVEL_ERROR, "[OS] Issue init MQTT.")
	return ERROR_IR_SETUP_FAIL;
}

/**
 * Setup the environment
 */
void setup(){

	/*
	 * Setup the various controllers
	 *  - Logging
	 *  - Ir
	 *  - Wifi
	 *  - Mqtt
	 */
	SETUP_LOGGING(115200);
	OS_PRINTLN(LEVEL_INFO, "[OS]: Setup Logging.");

	/*
	 * Setup IR
	 */
	if(ERROR_OK != (system_error = setupIr())){
		return;
	}
	else{

		/*
		 * Setup Wifi
		 */
		if(ERROR_OK != (system_error = setupWifi())){
			return;
		}
		else {

			/*
			 * Setup Mqtt
			 */
			if(ERROR_OK != (system_error = setupMqtt())){
				return;
			}
			else {
				OS_PRINTLN(LEVEL_INFO, "[OS] Setup Complete")
			}
		}
	}
}

/**
 * The main loop
 */
void loop() {

	// Check the setup
	if(ERROR_OK != system_error){

		OS_PRINTLN(LEVEL_ERROR, "[OS] Setup Failed")
		return;
	}
	else{
		// Get the interface and run the mqtt interface
		mqtt = get_MqttIface();
		if(ERROR_OK != serve_MqttIface()){

			OS_PRINTLN(LEVEL_ERROR, "[OS] Mqtt Failed")
			return;
		}
	}
}

