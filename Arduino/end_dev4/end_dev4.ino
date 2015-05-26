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

#include "DallasTemperature.h"
#include "DHT.h"
#include "Adafruit_MPL115A2.h"
#include "Wire.h"
#include "OneWire.h"
#include "Adafruit_BMP085.h"
#include "GenericSensor.h"
#include "SoftwareSerial.h"
#include "XBee.h"


#define PL_TEMPERATURE	(1<<0)
#define PL_HUMIDITY		(1<<1)
#define PL_PRESSURE		(1<<2)
#define PL_LIGHT		(1<<3)
#define PL_ALTITUDE		(1<<4)
#define PL_NODE			((2)<<5)

#define SOFTRX		A2
#define SOFTTX		A1
#define XBSLEEP		A0


uint32_t samplingTime = 30 * 1000;
uint32_t currentTime  = samplingTime;
uint32_t previousTime = 0;

float temp1, pres1, alt1;
uint8_t payload[13];

GenericSensor sensor1(BMP085, NULL);
SoftwareSerial serialSW(SOFTRX, SOFTTX);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();


void flashLed(int pin, int times, int wait);
void packFloat(float f, uint8_t *c);
float unpackFloat(uint8_t *c);

void setup() {	
	Serial.begin(9600);
	serialSW.begin(9600);
	xbee.setSerial(serialSW);
	Serial.println("Software starting..");

	sensor1.enableDebug(Serial);
	sensor1.init();
}


void loop() {	
	currentTime = millis();
	if ((currentTime-previousTime) > samplingTime) {
		previousTime = currentTime;

		
		payload[0] = (PL_NODE | PL_TEMPERATURE | PL_PRESSURE | PL_ALTITUDE);

		if(sensor1.readValue(TEMPERATURE, &temp1)) Serial.print("\nBMP data acq. succesful: ");
		packFloat(temp1, payload+1);
		Serial.println(temp1, 2);

		if(sensor1.readValue(PRESSURE, &pres1)) Serial.print("BMP data acq. succesful: ");
		packFloat(pres1, payload+5);
		Serial.println(pres1, 2);

		if(sensor1.readValue(ALTITUDE, &alt1)) Serial.print("BMP data acq. succesful: ");
		packFloat(alt1, payload+9);
		Serial.println(alt1, 2);

		pinMode(XBSLEEP, OUTPUT);
		digitalWrite(XBSLEEP, LOW);
		delay(2000);

		xbee.send(zbTx);

		Serial.print("Packet sent. Content: [ ");
		for(int i=0; i<12; i++) {
			Serial.print(payload[i], HEX);
			Serial.print(" ");
		}
		Serial.println("]");

		if (xbee.readPacket(500)) {
			// got a response!
			Serial.print("Received response: ");

			// should be a znet tx status            	
			if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
				xbee.getResponse().getZBTxStatusResponse(txStatus);

				// get the delivery status, the fifth byte
				if (txStatus.getDeliveryStatus() == SUCCESS) {
					Serial.println("succesful!");
				} else {
					Serial.println("fail!");
				}
			} else if (xbee.getResponse().isError()) {
				Serial.print("error code ");  
				Serial.println(xbee.getResponse().getErrorCode());
			} else {
				Serial.print("unknown packet ");  
				Serial.println(xbee.getResponse().getApiId());
			}
		} else {
			Serial.println("Local XBee timeout!");
		}

		pinMode(XBSLEEP, INPUT);
		digitalWrite(XBSLEEP, HIGH);
	}
}


void packFloat(float f, uint8_t *c) {
	memcpy(c, &f, 4);
}
