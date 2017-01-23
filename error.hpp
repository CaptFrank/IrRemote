/*
 * error.h
 *
 *  Created on: Jan 22, 2017
 *      Author: francispapineau
 */

#ifndef IRCONTROLLER_ERROR_HPP_
#define IRCONTROLLER_ERROR_HPP_

/**
 * This enum defines the global OS error codes.
 * All modules should include this file to get the
 * various errors.
 */
typedef enum {


	/*
	 * Global Errors
	 */
	ERROR_OK,                 //!< ERROR_OK
	ERROR_FAIL,               //!< ERROR_FAIL
	ERROR_NULL_PTR,           //!< ERROR_NULL_PTR

	/*
	 * IR Controller Errors
	 */
	ERROR_NOT_ACTIVATED,      //!< ERROR_NOT_ACTIVATED
	ERROR_COMMAND_NOT_VALID,  //!< ERROR_COMMAND_NOT_VALID
	ERROR_NOT_SUPPORTED_IOCTL,//!< ERROR_NOT_SUPPORTED_IOCTL
	ERROR_NO_MATCH,           //!< ERROR_NO_MATCH
	ERROR_IR_SETUP_FAIL,

	/*
	 * WIFI/MQTT Controller Errors
	 */
	ERROR_TIMEOUT,
	ERROR_CONNECTION,
	ERROR_SUBSCRIBING,
	ERROR_PUBLISHING,
	ERROR_WIFI_SETUP_FAIL,
	ERROR_MQTT_SETUP_FAIL

} error_t;


#endif /* IRCONTROLLER_ERROR_HPP_ */
