/*
 * configs.h
 *
 *  Created on: Jan 22, 2017
 *      Author: francispapineau
 */

#ifndef IRCONTROLLER_CONFIGS_HPP_
#define IRCONTROLLER_CONFIGS_HPP_

//! Includes
#include <Energia.h>

/**
 * This defines the projects configurations.
 * We should be including this file into each module to
 * get access to all defines and configurations.
 */

/**
 * The debug flag
 */
#define DEBUG_ON							(false)

/**
 * The printing shortcuts
 */
#define PRINT								Serial.print
#define PRINTLN								Serial.println
#define SETUP_LOGGING						Serial.begin;

/**
 * The printing levels.
 */
#define LEVEL_INFO							("[INFO]: ")
#define LEVEL_ERROR							("[ERROR]: ")
#define LEVEL_DEBUG							("[DEBUG]: ")

/**
 * Macro to print
 */
#define OS_PRINTLN(level, string) \
	PRINTLN(String(level) + String(string));
#define OS_PRINT(level, string) \
	PRINT(String(level) + String(string));

/**
 * LEDs
 */
#define WIFI_STATUS_LED						(GREEN_LED)
#define MQTT_STATUS_LED						(YELLOW_LED)

/**
 * System defines
 */
#define DEFAULT_DELAY						(200)

/**
 * The Wifi configurations
 */
#define WIFI_SSID							("Haligonian's Wi-Fi Network")
#define WIFI_PASSWORD						("Garf2345")
#define WIFI_IP								(0x9801000A) // "10.0.1.152"
#define WIFI_LOCAL_DNS1				       	(0x1101000A)
#define WIFI_LOCAL_DNS2				       	(0x0101000A)
#define WIFI_LOCAL_DNS_STR			     	("10.0.1.17, 10.0.1.1")

/**
 * The Mqtt configurations
 */
#define MQTT_MAX_PACKET_SIZE				(128)
#define MQTT_BROKER_ADDR			       	("mqtt.haligonia.home.com")
#define MQTT_BROKER_PORT			       	(1883)
#define MQTT_SYSTEM_ID						("SmartIrRemote")
#define MQTT_SUB_TOPIC						("SmartIrRemoteSub")
#define MQTT_PUB_TOPIC						("SmartIrRemotePub")

#define MQTT_ACK_RESPONSE					("OK: %d")
#define MQTT_ACK_SYSTEM						("OK: System")

#endif /* IRCONTROLLER_CONFIGS_HPP_ */
