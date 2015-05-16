/**
 * Copyright (c) 2015 Riccardo Miccini, Abed Shoka. All rights reserved.
 *
 * This file is part of SustainView.
 *
 * SustainView is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SustainView is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SustainView.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SoftwareSerial.h"
#include <XBee.h>

#define SOFTRX		10
#define SOFTTX		11
#define STATUSLED	13
#define ERRORLED	13


float temp1;
uint8_t *payload;


SoftwareSerial serialSW(SOFTRX, SOFTTX);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0xFFFF);
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();


void flashLed(int pin, int times, int wait);
void packFloat(float f, uint8_t *c);
float unpackFloat(uint8_t *c);
// bool uploadData(paremeters);

void setup() {
	pinMode(STATUSLED, OUTPUT);
	pinMode(ERRORLED, OUTPUT);

	serialSW.begin(9600);
	Serial.begin(9600);
	xbee.setSerial(Serial);
}


void loop() {
	xbee.readPacket();

	if (xbee.getResponse().isAvailable()) {
		serialSW.println("\nReceived packet!");

		if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
			serialSW.println("- it's a RX message");
			xbee.getResponse().getZBRxResponse(rx);

			if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
				serialSW.println("- it's been acknowledged");
				flashLed(STATUSLED, 5, 10);
			} else {
				serialSW.println("- it hasn't been acknowledged");
				flashLed(ERRORLED, 2, 20);
			}

			payload = rx.getData();
			float val1 = unpackFloat(payload);
			serialSW.print("- content: [ ");
			for(int i=0; i<4; i++) {
				serialSW.print(payload[i], HEX);
				serialSW.print(" ");
			}
			serialSW.print("]");
			serialSW.print("- sample value: ");
			serialSW.print(val1, 2);
			serialSW.println("");
			/*if(uploadData()) {
				serialSW.println("- pushed to server");
			} else {
				serialSW.println("- error pushing to server");
			}*/

		} else {
			serialSW.println("- it's ");
			flashLed(ERRORLED, 2, 50);
		}
	} else if (xbee.getResponse().isError()) {
		serialSW.print("- it's an error. Code: ");  
		serialSW.println(xbee.getResponse().getErrorCode());
	}
}


void flashLed(int pin, int times, int wait) {
	for (int i = 0; i < times; i++) {
		digitalWrite(pin, HIGH);
		delay(wait);
		digitalWrite(pin, LOW);
		if (i+1 < times)	delay(wait);
	}
}
/*
void packFloat(float f, uint8_t *c) {
	memcpy(c, &f, 4);
}
*/
float unpackFloat(uint8_t *c) {
	float f;
	memcpy(&f, c, 4);
	return f;
}