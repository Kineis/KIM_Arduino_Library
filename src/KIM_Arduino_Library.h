// -------------------------------------------------------------------------- //
//! @file		KIM_Arduino_Library.h
//! @brief		Handles the interface between KIM interface shield and Arduino
//! @note		Board compatibility :
//!					- Arduino UNO
//!					- Arduino UNO Wifi Rev2
//!					- Wemos D1
//!					- Arduino MKR Wifi 1010
//!					- Arduino NANO 33 BLE
//! @author		Kineis
//! @date		2022-02-03
//! @version	0.0.1
// -------------------------------------------------------------------------- //

#ifndef KIM_ARDUINO_LIBRARY_H
#define KIM_ARDUINO_LIBRARY_H

// -------------------------------------------------------------------------- //
// Includes
// -------------------------------------------------------------------------- //
#include <stdbool.h>
#include "Arduino.h"


// -------------------------------------------------------------------------- //
// Defines
// -------------------------------------------------------------------------- //
#define BAUDRATE 9600

#if defined(ESP8266)				//< WeMos D1

#define USER_INT_KIM	D3
#define ON_OFF_KIM		D4
#define RX_KIM			D7
#define TX_KIM			D8
#define softSerial		true

#elif defined(__AVR_ATmega4809__)	//< Uno Wifi Rev2

#define USER_INT_KIM	3
#define ON_OFF_KIM		4
#define RX_KIM			0
#define TX_KIM			1
#define softSerial		false

#elif defined(__AVR_ATmega328P__)	//< Uno

#define USER_INT_KIM	3
#define ON_OFF_KIM		4
#define RX_KIM			7
#define TX_KIM			8
#define softSerial		true

#elif defined(__SAMD21G18A__)		//MKR Wifi 1010

#define USER_INT_KIM	3
#define ON_OFF_KIM		1
#define RX_KIM			13
#define TX_KIM			14
#define softSerial		false

#else								//< nano 33 ble
			
#define USER_INT_KIM	3
#define ON_OFF_KIM		2
#define RX_KIM			0
#define TX_KIM			1
#define softSerial		false

//> Edit the following lines to add other boards :

//#elif defined(xxx)

//#define USER_INT_KIM		x
//#define ON_OFF_KIM		x
//#define RX_KIM			x
//#define TX_KIM			x
//#define softSerial		x

#endif

#if softSerial
	#include <SoftwareSerial.h>
	#include <new>
	#define SerialUART	SoftwareSerial
#else
	#define SerialUART	HardwareSerial
#endif


// -------------------------------------------------------------------------- //
// Types
// -------------------------------------------------------------------------- //
typedef enum {
	KIM_OK				= 0x00,		//!< OK return by KIM
	KIM_ERROR			= 0x01,		//!< Error returned by KIM
	KIM_TIMEOUT			= 0x02,		//!< Timeout, nothing returned by KIM
	UNKNOWN				= 0x04		//!< Unknown
} retStatusKIM_t;


// -------------------------------------------------------------------------- //
// Class
// -------------------------------------------------------------------------- //
class KIM {

private:
	retStatusKIM_t kimState;
	SerialUART *kimSerial;
	bool isKIM2 = true;

	char response[20];
	char command[70];


	//This syntaxe is used to be compatible with old versions of gcc
	char AT_REQUEST[5]	= {'=', '?', '\r', '\n', '\0'};	//!< "=?\r\0"
	char AT[3]			= {'A', 'T', '+'};				//!< "AT+"


// -------------------------------------------------------------------------- //
//! @brief	This function sends AT command to the KIM module.
//!
//!		The AT command is stored in 'command'
//!		The response is stored in 'response'
// -------------------------------------------------------------------------- //
	retStatusKIM_t KIM_sendUART();


public:
// -------------------------------------------------------------------------- //
//! @brief	Constructors
//!
//! @param [in] SerialUART* device
// -------------------------------------------------------------------------- //
	KIM(SerialUART *device);


// -------------------------------------------------------------------------- //
//! @brief	This function sends AT command to the KIM module.
//! @note	Command type : Get
//!
//! @param [in] ATcmd
//!		char array (ex: "ID") 
//! @param [in] len
//!		length of the char array (ex: 2) 
//!
//! @return	char *
//!		pointer to the response char array
// -------------------------------------------------------------------------- //
	char *KIM_sendATCommandGet(char ATcmd[], uint8_t len);


// -------------------------------------------------------------------------- //
//! @brief	This function sends AT command to the KIM module.
//! @note	Command type : Set
//!
//! @param [in] ATcmd
//!		char array (ex: "PWR") 
//! @param [in] len
//!		length of the char array (ex: 3)
//! @param [in] ATcmd parameter
//!		char array (ex: "500") 
//! @param [in] len
//!		length of the parameter char array (ex: 3)
//!
//! @return	char *
//!		pointer to the response char array
// -------------------------------------------------------------------------- //
	char *KIM_sendATCommandSet(char ATcmd[], uint8_t len, char ATcmdValue[], uint8_t valueLen);

	void KIM_userWakeupPinToggle();
	void KIM_powerON(bool powerON);
	retStatusKIM_t KIM_getState();
	bool KIM_isKIM2();

// -------------------------------------------------------------------------- //
//! @brief	Check the communication with the KIM module
//!
//! @return bool
//!		false	: check fail
//!		true	: check success
// -------------------------------------------------------------------------- //
	bool KIM_check(void);


// -------------------------------------------------------------------------- //
//! @brief Send raw data to KIM UART bus.
//!
//! @param [in] char[]
//!		char array (ex: "PWR") 
//! @param [in] len
//!		length of the input
//! @return char *
//!		pointer to the response char array
//!
//! @note Last chars of cmd[] must be "\r\n\0"
// -------------------------------------------------------------------------- //
	char *KIM_sendUARTRaw(char cmd[], uint8_t len);


// -------------------------------------------------------------------------- //
//! @brief Binary uint8_t[] to hex-string (char[]) convert function.
//!
//! @param [in] *input
//!		pointer to the input uint8_t array
//! @param [in] len
//!		length of the input array
//! @param [out] *output
//!		pointer to the output char array
// -------------------------------------------------------------------------- //
	static void KIM_uint2hexString(uint8_t *input, uint8_t len, char *output);
};

#endif //> KIM_ARDUINO_LIBRARY_H
