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


#define SOFTRX		10
#define SOFTTX		11
#define STATUSLED	13
#define ERRORLED	13
#define ONEWIRE_PIN	2



uint32_t samplingTime = 5 * 1000;
uint32_t currentTime  = samplingTime;
uint32_t previousTime = 0;

float temp1;
uint8_t pinSetting1[] = {ONEWIRE_PIN};
uint8_t payload[4];


GenericSensor sensor1(DALLAS_DS18B20, pinSetting1);
SoftwareSerial serialSW(SOFTRX, SOFTTX);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();


void flashLed(int pin, int times, int wait);
void packFloat(float f, uint8_t *c);
float unpackFloat(uint8_t *c);

void setup() {
	sensor1.enableDebug(&serialSW);

	pinMode(STATUSLED, OUTPUT);
	pinMode(ERRORLED, OUTPUT);

	serialSW.begin(9600);
	Serial.begin(9600);
	xbee.setSerial(Serial);
}


void loop() {
	currentTime = millis();
	if ((currentTime-previousTime) > samplingTime) {
		previousTime = currentTime;

		sensor1.readValue(TEMPERATURE, &temp1);
		temp1 = random(-100, 400)/10.0;
		packFloat(temp1, payload);
		serialSW.print("New sample. Content: ");
		serialSW.print(temp1, 2);
		serialSW.println("");

		xbee.send(zbTx);
		flashLed(STATUSLED, 1, 100);

		serialSW.print("Packet sent. Content: [ ");
		for(int i=0; i<4; i++) {
			serialSW.print(payload[i], HEX);
			serialSW.print(" ");
		}
		serialSW.println("]");


		if (xbee.readPacket(500)) {
			// got a response!
			serialSW.println("Received response!");

			// should be a znet tx status            	
			if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
				serialSW.println("- it's a status response");

				xbee.getResponse().getZBTxStatusResponse(txStatus);

				// get the delivery status, the fifth byte
				if (txStatus.getDeliveryStatus() == SUCCESS) {
					serialSW.println("- it's succesful");
					flashLed(STATUSLED, 5, 50);
				} else {
					serialSW.println("- it wasn't succesful");
					flashLed(ERRORLED, 3, 500);
				}
			} else if (xbee.getResponse().isError()) {
				serialSW.print("- it's an error. Code: ");  
				serialSW.println(xbee.getResponse().getErrorCode());
			}
		} else {
			// local XBee did not provide a timely TX Status Response -- should not happen
			serialSW.println("Local XBee timeout!");
			flashLed(ERRORLED, 2, 50);
		}
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

void packFloat(float f, uint8_t *c) {
	memcpy(c, &f, 4);
}
/*
float unpackFloat(uint8_t *c) {
	float f;
	memcpy(&f, c, 4);
	return f;
}
*/