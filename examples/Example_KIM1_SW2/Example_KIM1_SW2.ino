// -------------------------------------------------------------------------- //
//! @brief	KIM2 library example 
//! @note	Example for KIM1 module (SW2.0 / SW2.1)
//! @note	Board compatibility :
//!					- Arduino UNO
//!					- Arduino UNO Wifi Rev2
//!					- Wemos D1
//!					- Arduino MKR Wifi 1010
//!					- Arduino NANO 33 BLE
// -------------------------------------------------------------------------- //

#include "KIM_Arduino_Library.h"

/*-- CONFIGURATION START --*/
#define TX_PERIOD 50000
char DATA[] = "0123456789";

/*-- CONFIGURATION END --*/

#define TX_DURATION_MS  6000
#define INTERVAL_MS     (TX_PERIOD-TX_DURATION_MS)

/*-- AT COMMANDS START --*/
char ID[2]  = "ID";
char FW[2]  = "FW";
char TX[2]  = "TX";
char PWR[3] = "PWR";
/*-- AT COMMANDS END --*/

#if defined(__AVR_ATmega328P__) or defined(ESP8266) // Arduino UNO and Wemos D1

SoftwareSerial kserial(RX_KIM, TX_KIM);

#else // Arduino UNO Wifi Rev2, Arduino MKR Wifi 1010 and Arduino NANO 33 BLE

HardwareSerial &kserial = Serial1;

#endif

KIM kim(&kserial);

void setup() {
	Serial.begin(9600);
	Serial.println();

	Serial.println("Example KIM1 Arduino shield");
	
	kim.KIM_powerON(true);
	kim.KIM_userWakeupPinToggle();

	if (kim.KIM_check()) {
		Serial.println("KIM1 -- Check success");
	} else {
		Serial.println("KIM1 -- Check fail. Please check wiring and jumpers. Freezing.");
		while(1)
			;
	}

	if (kim.KIM_isKIM2() == true)
		Serial.println("KIM2 -- KIM2 module detected");
	else
		Serial.println("KIM1 -- KIM1 module detected");

	Serial.print("KIM1 -- Get ID : ");
	Serial.println(kim.KIM_sendATCommandGet(ID, sizeof(ID)));
	Serial.print("KIM1 -- Get FW : ");
	Serial.println(kim.KIM_sendATCommandGet(FW, sizeof(FW)));

	/* Set TX configuration */
	kim.KIM_sendATCommandSet(PWR, sizeof(PWR), "500", sizeof("500") - 1);
	Serial.print("KIM1 -- Get PWR : ");
	Serial.println(kim.KIM_sendATCommandGet(PWR, sizeof(PWR)));
}

void loop() {
	Serial.print("KIM1 -- Send data ... ");

	kim.KIM_sendATCommandSet(TX, sizeof(TX), DATA, sizeof(DATA) - 1);
	if (kim.KIM_getState() == KIM_OK)
		Serial.println("Message sent");
	else
		Serial.println("Error");

	Serial.println("KIM1 -- Turn OFF");

	kim.KIM_powerON(false);
	delay(INTERVAL_MS + random(-0.1 * TX_PERIOD, 0.1 * TX_PERIOD));
	kim.KIM_powerON(true);
}