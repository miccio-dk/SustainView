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

#include "XBeeHandler.h"
#include "GenericSensor.h"


#define STATUSLED	13
#define ERRORLED	13
#define DATALED		13
#define SOFTRX		10
#define SOFTTX		11
#define ONEWIRE_PIN	2

#define MAX_NODES	10



XBeeHandler xbeeHandler(STATUSLED, ERRORLED, DATALED, SOFTRX, SOFTTX);
XBeeNode list[MAX_NODES];


uint8_t pinSetting[] = {ONEWIRE_PIN};		// Dallas sensor: only needs one data pin
GenericSensor sensor1(DALLAS_DS18B20, pinSetting);


uint32_t samplingTime = 3 * 1000;
uint32_t currentTime  = samplingTime;
uint32_t previousTime = 0;


void setup() {
	// TODO(riccardo) following line disabled for testing sensors library; re-enable when needed
 	// xbeeHandler.begin();

}


void loop() {
//  currentTime = millis();
//  if ((currentTime-previousTime) > samplingTime) {
//    previousTime = currentTime;

	// example usage of of the sensor Library
	int16_t val1 = 0;
	float 	val2 = 0;
	sensor1.readValue(TEMPERATURE, &val2);

	// example susage of the xBeeHandler library
	// xbeeHandler.discover(list, MAX_NODES);



	while(true);
	// TODO all this stuff in the XBeeHandler class
	// coordinator.send(zbTxReq);
	// coordinator.send(atTxReq);
	// mySerial.println("Sending request..");
//  }

  //xbeeHandler.update();
}
