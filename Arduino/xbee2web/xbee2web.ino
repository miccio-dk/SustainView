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

#include <SoftwareSerial.h>
#include "XBeeHandler.h"


#define STATUSLED 13
#define ERRORLED  13
#define DATALED   13
#define SOFTRX    10
#define SOFTTX    11


XBeeHandler xbeeHandler(STATUSLED, ERRORLED, DATALED, SOFTRX, SOFTTX);

uint32_t currentTime  = 0;
uint32_t previousTime = 0;
uint32_t samplingTime = 3 * 1000;


void setup() {
  xbeeHandler.begin();
}


void loop() {
  currentTime = millis();
  if ((currentTime-previousTime) > samplingTime) {
    previousTime = currentTime;

    // TODO all this stuff in the XBeeHandler class
    // coordinator.send(zbTxReq);
    // coordinator.send(atTxReq);
    // mySerial.println("Sending request..");
  }

  xbeeHandler.update();
}
