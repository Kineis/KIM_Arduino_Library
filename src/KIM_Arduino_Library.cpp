// -------------------------------------------------------------------------- //
//! @file		KIM_Arduino_Library.cpp
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


// -------------------------------------------------------------------------- //
// Includes
// -------------------------------------------------------------------------- //
#include "KIM_Arduino_Library.h"


// -------------------------------------------------------------------------- //
//! Constructor
// -------------------------------------------------------------------------- //
KIM::KIM(SerialUART *device)
{
	kimSerial = device;

	pinMode(USER_INT_KIM, OUTPUT);
	pinMode(ON_OFF_KIM, OUTPUT);
	pinMode(RX_KIM, OUTPUT);
	pinMode(TX_KIM, INPUT);

	digitalWrite(USER_INT_KIM, LOW);
	digitalWrite(ON_OFF_KIM, LOW);

	kimState = UNKNOWN;
}


// -------------------------------------------------------------------------- //
//! Toggle WU pin
// -------------------------------------------------------------------------- //
void KIM::KIM_userWakeupPinToggle()
{
	digitalWrite(USER_INT_KIM, HIGH);
	delay(50);
	digitalWrite(USER_INT_KIM, LOW);
}


// -------------------------------------------------------------------------- //
//! Turn ON or OFF KIM module
// -------------------------------------------------------------------------- //
void KIM::KIM_powerON(bool powerON)
{
	if (powerON) {
		digitalWrite(ON_OFF_KIM, powerON);
#if softSerial
		new (kimSerial) SerialUART(RX_KIM, TX_KIM);	//Initialize UART
#endif
		kimSerial->begin(BAUDRATE);
		kimSerial->flush();					//Clean TX buffer
		while (kimSerial->available() > 0)	//Clean RX buffer
			kimSerial->read();
			
		delay(10);
	} else {
		kimSerial->flush();					//Clean TX buffer
		while (kimSerial->available() > 0)	//Clean RX buffer
			kimSerial->read();

		digitalWrite(ON_OFF_KIM, powerON);
#if softSerial
		kimSerial->~SerialUART();			//De-initialize UART
#else
		kimSerial->end();
#endif
	}
}


// -------------------------------------------------------------------------- //
//! Toggle WU pin and check KIM module
// -------------------------------------------------------------------------- //
bool KIM::KIM_check()
{
	KIM_userWakeupPinToggle();

	KIM_sendATCommandGet("PING", 4);

	if (response[0] != '+') {
		if (isKIM2 == true) {
			isKIM2 = false;
			AT_REQUEST[3] = '\0';
		} else {
			isKIM2 = true;
			AT_REQUEST[3] = '\n';
		}
		KIM_sendATCommandGet("PING", 4);
	}

	if (KIM_getState() == KIM_OK)
		return true;
	else
		return false;
}


// -------------------------------------------------------------------------- //
//! Returns the KIM module last State (Updated when a command is sent)
// -------------------------------------------------------------------------- //
retStatusKIM_t KIM::KIM_getState()
{
	return kimState;
}


// -------------------------------------------------------------------------- //
//! Returns the KIM module nature (true : KIM2 ; false : KIM1)
// -------------------------------------------------------------------------- //
bool KIM::KIM_isKIM2()
{
	return isKIM2;
}


// -------------------------------------------------------------------------- //
//! This function sends AT command request to the KIM module.
// -------------------------------------------------------------------------- //
char *KIM::KIM_sendATCommandGet(char ATcmd[], uint8_t len)
{
	// Request AT command starts with "AT+"
	for (uint8_t i = 0; i < sizeof(AT); i++)
		command[i] = AT[i];

	// AT command type
	for (uint8_t i = 0; i < len; i++)
		command[i + sizeof(AT)] = ATcmd[i];

	// AT command request ends with "=?\r\0"
	for (uint8_t i = 0; i < sizeof(AT_REQUEST); i++)
		command[i + sizeof(AT) + len] = AT_REQUEST[i];

	kimState = KIM_sendUART();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! This function sends AT command to the KIM module.
// -------------------------------------------------------------------------- //
char *KIM::KIM_sendATCommandSet(char ATcmd[], uint8_t len, char ATcmdValue[], uint8_t valueLen)
{
	// Request AT command starts with "AT+"
	for (uint8_t i = 0; i < sizeof(AT); i++)
		command[i] = AT[i];

	// AT command type
	for (uint8_t i = 0; i < len; i++)
		command[i + sizeof(AT)] = ATcmd[i];

	command[sizeof(AT) + len] = '=';

	// AT command value
	for (uint8_t i = 0; i < valueLen; i++)
		command[i + sizeof(AT) + len + 1] = ATcmdValue[i];

	// AT command ends with "\r\0"
	for (uint8_t i = 0; i < sizeof(AT_REQUEST) - 2; i++)
		command[i + sizeof(AT) + len + 1 + valueLen] = AT_REQUEST[i + 2];

	kimState = KIM_sendUART();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! This function sends command to the KIM module.
// -------------------------------------------------------------------------- //
retStatusKIM_t KIM::KIM_sendUART()
{
	bool delayedResponse = false;
	uint8_t i = 0;

	kimSerial->flush();					//Clean TX buffer
	while (kimSerial->available() > 0)	//Clean RX buffer
		kimSerial->read();

	if(command[3] == 'T' && command[4] == 'X')
		delayedResponse = true;

	if (delayedResponse == false)
		i = 5;

	response[0] = '\0';
	kimSerial->print(command);

	for ( ; i < 10; i++) {
		uint8_t k = kimSerial->readBytesUntil('\n', response, 20);
		response[k] = '\0';

		if (response[0] == '+') {
			if (response[1] != 'E' && response[2] == 'R')/*+ERROR*/
				return KIM_ERROR;
			else 
				if (delayedResponse == false)
					return KIM_OK;
				else
					delayedResponse = false;
		}
	}
	return KIM_TIMEOUT;
}


// -------------------------------------------------------------------------- //
//! Send raw data to KIM UART bus.
// -------------------------------------------------------------------------- //
char *KIM::KIM_sendUARTRaw(char RAWcmd[], uint8_t len)
{
	for (uint8_t i = 0; i < len - 3; i++)
		command[i] = RAWcmd[i];

	// Be sure that command ends with "\r\0"
	command[len - 3] = '\r';
	command[len - 2] = '\n';
	command[len - 1] = '\0';

	kimState = KIM_sendUART();
	return &response[0];
}


// -------------------------------------------------------------------------- //
//! Binary uint8_t[] to hex-string (char[]) convert function.
// -------------------------------------------------------------------------- //
void KIM::KIM_uint2hexString(uint8_t *input, uint8_t len, char *output)
{
	uint8_t idx = 0;

	for (idx = 0; idx < len; idx++)
		sprintf((char *)(output + idx * 2), "%02X", input[idx]);
}
