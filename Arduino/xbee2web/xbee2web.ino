/**
 * Copyright (c) 2015 Riccardo Miccini. All rights reserved.
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

#include "XBee.h"
#include <SoftwareSerial.h>


SoftwareSerial mySerial(10, 11);

int statusLed = 13;
int errorLed = 13;
int dataLed = 13;

XBee coordinator = XBee();
XBeeAddress64 addrBroadcast = XBeeAddress64(0x00000000, ZB_BROADCAST_ADDRESS);

uint8_t payload[] = {0, 0};
uint8_t cmdMY[] = {'M', 'Y'};
uint8_t cmdNI[] = {'N', 'I'};
ZBTxRequest zbTx = ZBTxRequest(addrBroadcast, payload, sizeof(payload));
RemoteAtCommandRequest atTx = RemoteAtCommandRequest(addrBroadcast, cmdNI);


XBeeResponse response = XBeeResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
ZBRxResponse zbRx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();
RemoteAtCommandResponse atRx = RemoteAtCommandResponse();

long currentTime = 0;
long previousTime = 0;
long samplingTime = 3 * 1000;


void flashLed(int pin, int times, int wait);


void setup() {
	pinMode(statusLed, OUTPUT);
	pinMode(errorLed, OUTPUT);
	pinMode(dataLed,  OUTPUT);

	mySerial.begin(9600);
	Serial.begin(9600);
	coordinator.begin(Serial);

	flashLed(statusLed, 3, 50);
}


void loop() {
	currentTime = millis();
	if((currentTime-previousTime) > samplingTime) {
		previousTime = currentTime;

		//coordinator.send(zbTx);
		coordinator.send(atTx);

		//Serial.print("Sending packet..");
	}

	coordinator.readPacket();
	if (coordinator.getResponse().isAvailable()) {
		uint8_t apiId =  coordinator.getResponse().getApiId();
		switch (apiId) {
			case ZB_RX_RESPONSE:
				coordinator.getResponse().getZBRxResponse(zbRx);
				if (zbRx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
					flashLed(statusLed, 10, 10);
				} else {
					flashLed(errorLed, 2, 20);
				}
				break;
			case MODEM_STATUS_RESPONSE:
				coordinator.getResponse().getModemStatusResponse(msr);
				if (msr.getStatus() == ASSOCIATED) {
					flashLed(statusLed, 10, 10);
				} else if (msr.getStatus() == DISASSOCIATED) {
					flashLed(errorLed, 10, 10);
				} else {
					flashLed(statusLed, 5, 10);
				}
				break;
			case REMOTE_AT_COMMAND_RESPONSE:
				coordinator.getResponse().getRemoteAtCommandResponse(atRx);
				if (atRx.isOk()) {
					flashLed(statusLed, 10, 10);

					if (atRx.getValueLength() > 0) {
						for (int i = 0; i < atRx.getValueLength(); i++) {
							mySerial.print(atRx.getValue()[i], HEX);
							mySerial.print(" ");
						}

						mySerial.println("");
					}
				} else {
					flashLed(errorLed, 1, 25);
					mySerial.print("Command returned error code: ");
					mySerial.println(atRx.getStatus(), HEX);
				}
				break;
			default:
				flashLed(errorLed, 1, 25);
				break;
		}
	} else if (coordinator.getResponse().isError()) {
		flashLed(errorLed, 1, 25);
		mySerial.print("Error reading packet.  Error code: ");  
		mySerial.println(coordinator.getResponse().getErrorCode());
	}
}


void flashLed(int pin, int times, int wait) {

	for (int i = 0; i < times; i++) {
		digitalWrite(pin, HIGH);
		delay(wait);
		digitalWrite(pin, LOW);

		if (i + 1 < times) {
			delay(wait);
		}
	}
}