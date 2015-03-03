// Copyright (c) 2015 Riccardo Miccini. All rights reserved.


#include "XBeeHandler.h"
#include "XBee.h"
#include <SoftwareSerial.h>


uint8_t payload[] = {0, 0};
uint8_t cmdNI[] = {'N', 'I'};



XBeeHandler::XBeeHandler(uint8_t statusLed, uint8_t errorLed, uint8_t dataLed, uint8_t rxPin, uint8_t txPin) {
  setDebugLeds(statusLed, errorLed, dataLed);
  setDebugSerial(rxPin, txPin);
  selectDebug(DEBUG_BOTH);

  init();
}

XBeeHandler::XBeeHandler(uint8_t statusLed, uint8_t errorLed, uint8_t dataLed) {
  setDebugLeds(statusLed, errorLed, dataLed);
  detachDebugLSerial();
  selectDebug(DEBUG_LEDS);

  init();
}

XBeeHandler::XBeeHandler(uint8_t rxPin, uint8_t txPin) {
  detachDebugLeds();
  setDebugSerial(rxPin, txPin);
  selectDebug(DEBUG_SERIAL);

  init();
}

XBeeHandler::XBeeHandler() {
  detachDebugLeds();
  detachDebugLSerial();
  selectDebug(DEBUG_NONE);

  init();
}

XBeeHandler::~XBeeHandler() {
  delete _serial;
  delete[] _data;
}


void XBeeHandler::selectDebug(DebugType type) {
  switch (type) {
    case DEBUG_NONE:   _debugSerial = false; _debugLeds = false; break;
    case DEBUG_SERIAL: _debugSerial = true;  _debugLeds = false; break;
    case DEBUG_LEDS:   _debugSerial = false; _debugLeds = true;  break;
    case DEBUG_BOTH:   _debugSerial = true;  _debugLeds = true;  break;
    default:           break;
  }
}

void XBeeHandler::setDebugSerial(uint8_t rxPin, uint8_t txPin) {
  _serial = new SoftwareSerial(rxPin, txPin);
  _serial->begin(9600);
  _serial->println("Serial debug is ON!");
  _debugSerial = true;
}

void XBeeHandler::setDebugLeds(uint8_t statusLed, uint8_t errorLed, uint8_t dataLed) {
  _statusLed = statusLed;
  _errorLed = errorLed;
  _dataLed = dataLed;
  _debugLeds = true;
}

void XBeeHandler::detachDebugLeds() {
  _statusLed = 0;
  _errorLed = 0;
  _dataLed = 0;
  _debugLeds = false;
}

void XBeeHandler::detachDebugLSerial() {
  if (_debugSerial) {delete _serial;}
  _debugSerial = false;
}


void XBeeHandler::begin() {
  if (_debugLeds) {
    pinMode(_statusLed, OUTPUT);
    pinMode(_errorLed, OUTPUT);
    pinMode(_dataLed,  OUTPUT);
    flashLed(STATUSLED, 3, 50);
  }

  Serial.begin(9600);
  _xbee.begin(Serial);
}

void XBeeHandler::update() {
  if (CheckPacketsOld()) {
    HandleResponse(_response);
  }
}

uint8_t XBeeHandler::discover(XBeeNode list[], uint8_t maxNodes) {
  uint8_t cmdNT[] = {'N', 'T'};
  uint8_t cmdND[] = {'N', 'D'};
  uint16_t timeOut = 0;
  uint8_t numNodes = 0;

  if (_debugSerial) _serial->println("### Discovering nodes..");
  if (_debugSerial) _serial->println("# Sending NT request..");
  _atCommandReq.setCommand(cmdNT);
  _xbee.send(_atCommandReq);

  while (!CheckPackets(5000)) {}
  HandleResponse(_response);
  timeOut = _data[0]*100;
  if (_debugSerial) _serial->print("# Timeout: ");
  if (_debugSerial) _serial->println(timeOut, DEC);

  if (_debugSerial) _serial->println("# Sending ND request..");
  _atCommandReq.setCommand(cmdND);
  _xbee.send(_atCommandReq);

  uint16_t startTime = millis();
  uint16_t endTime = 0;
  while ((endTime-startTime) <= 5000) {  // timeOut
    if (CheckPacketsOld()) {
      HandleResponse(_response);
      for (int i = 0; i < _dataLen; i++) {
        if (_debugSerial) _serial->print(_data[i], HEX);
        if (_debugSerial) _serial->print(" ");
      }
      if (_debugSerial) _serial->println();
    }
  }

  return numNodes;
}



boolean XBeeHandler::CheckPacketsOld() {
  boolean packetFound = false;
  _xbee.readPacket();
  _response = _xbee.getResponse();

  if (_response.isAvailable()) {
    packetFound = true;
  } else if (_response.isError()) {
    if (_debugLeds)   flashLed(ERRORLED, 1, 25);
    if (_debugSerial) _serial->print("::CheckPackets - Command returned error code: ");
    if (_debugSerial) _serial->println(_response.getErrorCode(), HEX);
  }
  return packetFound;
}

boolean XBeeHandler::CheckPackets(uint16_t time) {
  boolean packetFound = false;

  if (_xbee.readPacket(time)) {
    _response = _xbee.getResponse();
    packetFound = true;
  } else {
    if (_response.isError()) {
      if (_debugLeds)   flashLed(ERRORLED, 1, 25);
      if (_debugSerial) _serial->print("::CheckPackets - Command returned error code: ");
      if (_debugSerial) _serial->println(_response.getErrorCode(), HEX);
    } else {
      if (_debugLeds)   flashLed(ERRORLED, 1, 25);
      if (_debugSerial) _serial->println("::CheckPackets - No response from the module.");
    }
  }

  return packetFound;
}


void XBeeHandler::HandleZBRxResponse(XBeeResponse &resp) {
  resp.getZBRxResponse(_zbRxResp);
  if (_zbRxResp.getOption() == ZB_PACKET_ACKNOWLEDGED) {
    flashLed(STATUSLED, 10, 10);
  } else {
    flashLed(ERRORLED, 2, 20);
  }
}

void XBeeHandler::HandleModemStatusResponse(XBeeResponse &resp) {
  resp.getModemStatusResponse(_modemStatusResp);
  if (_modemStatusResp.getStatus() == ASSOCIATED) {
    flashLed(STATUSLED, 10, 10);
  } else if (_modemStatusResp.getStatus() == DISASSOCIATED) {
    flashLed(ERRORLED, 10, 10);
  } else {
    flashLed(STATUSLED, 5, 10);
  }
}

void XBeeHandler::HandleRemoteAtCommandResponse(XBeeResponse &resp) {
  resp.getRemoteAtCommandResponse(_remoteAtCommandResp);
  if (_remoteAtCommandResp.isOk()) {
    flashLed(STATUSLED, 10, 10);

    if (_remoteAtCommandResp.getValueLength() > 0) {
      delete[] _data;
      _data      = _remoteAtCommandResp.getValue();
      _dataLen   = _remoteAtCommandResp.getValueLength();
      _dataEmpty = false;

//      for (int i = 0; i < _remoteAtCommandResp.getValueLength(); i++) {
//        _serial->print(static_cast<char>(_remoteAtCommandResp.getValue()[i]));
//      }
//      _serial->println("");
    }
  } else {
    if (_debugLeds)   flashLed(ERRORLED, 1, 25);
    if (_debugSerial) _serial->print("::HandleRemoteAtCommandResponse - Command returned error code: ");
    if (_debugSerial) _serial->println(_remoteAtCommandResp.getStatus(), HEX);
  }
}

void XBeeHandler::HandleAtCommandResponse(XBeeResponse &resp) {
  resp.getAtCommandResponse(_atCommandResp);
  if (_atCommandResp.isOk()) {
    flashLed(STATUSLED, 10, 10);

    if (_atCommandResp.getValueLength() > 0) {
      delete[] _data;
      _data      = _atCommandResp.getValue();
      _dataLen   = _atCommandResp.getValueLength();
      _dataEmpty = false;

//      for (int i = 0; i < _atCommandResp.getValueLength(); i++) {
//        _serial->print(static_cast<char>(_atCommandResp.getValue()[i]));
//      }
//      _serial->println("");
    }
  } else {
    if (_debugLeds)   flashLed(ERRORLED, 1, 25);
    if (_debugSerial) _serial->print("::HandleAtCommandResponse - Command returned error code: ");
    if (_debugSerial) _serial->println(_atCommandResp.getStatus(), HEX);
  }
}

void XBeeHandler::HandleUnknown(XBeeResponse &resp) {
  if (_debugLeds) flashLed(ERRORLED, 1, 25);
  if (_debugSerial) _serial->print("::HandleUnknown - Unknown ApiID: 0x");
  if (_debugSerial) _serial->println(resp.getApiId(), HEX);
}

void XBeeHandler::HandleResponse(XBeeResponse &resp) {
  uint8_t apiId =  resp.getApiId();
  switch (apiId) {
    case ZB_RX_RESPONSE:             HandleZBRxResponse(resp);            break;
    case MODEM_STATUS_RESPONSE:      HandleModemStatusResponse(resp);     break;
    case REMOTE_AT_COMMAND_RESPONSE: HandleRemoteAtCommandResponse(resp); break;
    case AT_COMMAND_RESPONSE:        HandleAtCommandResponse(resp);       break;
    default:                  break;
  }
}

void XBeeHandler::flashLed(LedType type, int times, int wait) {
  uint8_t types[] = {_statusLed, _dataLed, _errorLed};
  uint8_t pin = types[type];

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

void XBeeHandler::init() {
  _xbee = XBee();
  _addr = RemoteAtCommandRequest::broadcastAddress64;

  _zbTxReq = ZBTxRequest();
  _atCommandReq = AtCommandRequest();
  _remoteAtCommandReq = RemoteAtCommandRequest();

  _response = XBeeResponse();
  _zbTxStatusResp = ZBTxStatusResponse();
  _zbRxResp = ZBRxResponse();
  _modemStatusResp = ModemStatusResponse();
  _atCommandResp = AtCommandResponse();
  _remoteAtCommandResp = RemoteAtCommandResponse();

  _data = new uint8_t[MAX_PAYLOAD];
}
