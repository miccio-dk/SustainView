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


#define statusLed	13
#define errorLed	13
#define dataLed		13

SoftwareSerial mySerial(10, 11);

XBee coordinator = XBee();

uint8_t payload[] = {0, 0};
uint8_t cmdMY[] = {'M', 'Y'};
uint8_t cmdNI[] = {'N', 'I'};
ZBTxRequest zbTx = ZBTxRequest(RemoteAtCommandRequest::broadcastAddress64, payload, sizeof(payload));
RemoteAtCommandRequest atTx = RemoteAtCommandRequest(RemoteAtCommandRequest::broadcastAddress64, cmdNI);


XBeeResponse response = XBeeResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
ZBRxResponse zbRx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();
RemoteAtCommandResponse atRx = RemoteAtCommandResponse();

long currentTime = 0;
long previousTime = 0;
long samplingTime = 3 * 1000;


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

		mySerial.println("Sending request..");
	}

	if(CheckPackets()) {
		DoCommand(response); 
	}

	
}


boolean CheckPackets() {
	boolean packetFound = false;
	coordinator.readPacket();
	response = coordinator.getResponse();

	if (response.isAvailable()) {
		packetFound = true;
	} else if (response.isError()) {
		flashLed(errorLed, 1, 25);
		mySerial.print("Error reading packet.  Error code: ");  
		mySerial.println(response.getErrorCode());
	}
	return packetFound;
}


boolean DoCommand(XBeeResponse &resp) {
	uint8_t apiId =  resp.getApiId();
	switch (apiId) {
		case ZB_RX_RESPONSE:
			resp.getZBRxResponse(zbRx);
			if (zbRx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
				flashLed(statusLed, 10, 10);
			} else {
				flashLed(errorLed, 2, 20);
			}
			break;
		case MODEM_STATUS_RESPONSE:
			resp.getModemStatusResponse(msr);
			if (msr.getStatus() == ASSOCIATED) {
				flashLed(statusLed, 10, 10);
			} else if (msr.getStatus() == DISASSOCIATED) {
				flashLed(errorLed, 10, 10);
			} else {
				flashLed(statusLed, 5, 10);
			}
			break;
		case REMOTE_AT_COMMAND_RESPONSE:
			resp.getRemoteAtCommandResponse(atRx);
			if (atRx.isOk()) {
				flashLed(statusLed, 10, 10);

				if (atRx.getValueLength() > 0) {
					for (int i = 0; i < atRx.getValueLength(); i++) {
						mySerial.print( (char)(atRx.getValue()[i]) );
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