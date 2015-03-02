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


#define statusLed 13
#define errorLed  13
#define dataLed   13

SoftwareSerial mySerial(10, 11);

XBee coordinator = XBee();

XBeeAddress64 broadcastAddr = RemoteAtCommandRequest::broadcastAddress64;

uint8_t payload[] = {0, 0};
uint8_t cmdMY[] = {'M', 'Y'};
uint8_t cmdNI[] = {'N', 'I'};
ZBTxRequest            zbTxReq = ZBTxRequest(broadcastAddr, payload, sizeof(payload));
RemoteAtCommandRequest atTxReq = RemoteAtCommandRequest(broadcastAddr, cmdNI);

XBeeResponse            response = XBeeResponse();
ZBTxStatusResponse      zbTxStatusResp = ZBTxStatusResponse();
ZBRxResponse            zbRxResp = ZBRxResponse();
ModemStatusResponse     modemStatusResp = ModemStatusResponse();
RemoteAtCommandResponse remoteAtCommandResp = RemoteAtCommandResponse();
AtCommandResponse       atCommandResp = AtCommandResponse();


uint32_t currentTime  = 0;
uint32_t previousTime = 0;
uint32_t samplingTime = 3 * 1000;


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
  if ((currentTime-previousTime) > samplingTime) {
    previousTime = currentTime;

    // coordinator.send(zbTxReq);
    coordinator.send(atTxReq);

    mySerial.println("Sending request..");
  }

  if (CheckPackets()) {
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
    mySerial.print("Error reading packet. Error code: ");
    mySerial.println(response.getErrorCode());
  }
  return packetFound;
}


void HandleZBRxResponse(XBeeResponse &resp) {
  resp.getZBRxResponse(zbRxResp);
  if (zbRxResp.getOption() == ZB_PACKET_ACKNOWLEDGED) {
    flashLed(statusLed, 10, 10);
  } else {
    flashLed(errorLed, 2, 20);
  }
}

void HandleModemStatusResponse(XBeeResponse &resp) {
  resp.getModemStatusResponse(modemStatusResp);
  if (modemStatusResp.getStatus() == ASSOCIATED) {
    flashLed(statusLed, 10, 10);
  } else if (modemStatusResp.getStatus() == DISASSOCIATED) {
    flashLed(errorLed, 10, 10);
  } else {
    flashLed(statusLed, 5, 10);
  }
}

void HandleRemoteAtCommandResponse(XBeeResponse &resp) {
  resp.getRemoteAtCommandResponse(remoteAtCommandResp);
  if (remoteAtCommandResp.isOk()) {
    flashLed(statusLed, 10, 10);

    if (remoteAtCommandResp.getValueLength() > 0) {
      for (int i = 0; i < remoteAtCommandResp.getValueLength(); i++) {
        mySerial.print(static_cast<double>(remoteAtCommandResp.getValue()[i]));
      }
      mySerial.println("");
    }
  } else {
    flashLed(errorLed, 1, 25);
    mySerial.print("Command returned error code: ");
    mySerial.println(remoteAtCommandResp.getStatus(), HEX);
  }
}

void HandleAtCommandResponse(XBeeResponse &resp) {
  resp.getAtCommandResponse(atCommandResp);
  if (atCommandResp.isOk()) {
    flashLed(statusLed, 10, 10);

    if (atCommandResp.getValueLength() > 0) {
      for (int i = 0; i < atCommandResp.getValueLength(); i++) {
        mySerial.print(static_cast<double>(atCommandResp.getValue()[i]));
      }
      mySerial.println("");
    }
  } else {
    flashLed(errorLed, 1, 25);
    mySerial.print("Command returned error code: ");
    mySerial.println(atCommandResp.getStatus(), HEX);
  }
}

boolean DoCommand(XBeeResponse &resp) {
  uint8_t apiId =  resp.getApiId();
  switch (apiId) {
    case ZB_RX_RESPONSE:             HandleZBRxResponse(resp);            break;
    case MODEM_STATUS_RESPONSE:      HandleModemStatusResponse(resp);     break;
    case REMOTE_AT_COMMAND_RESPONSE: HandleRemoteAtCommandResponse(resp); break;
    case AT_COMMAND_RESPONSE:        HandleAtCommandResponse(resp);       break;
    default:                         flashLed(errorLed, 1, 25);           break;
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
