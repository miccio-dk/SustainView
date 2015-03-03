// Copyright (c) 2015 Riccardo Miccini. All rights reserved.

#ifndef XBeeHandler_H
#define XBeeHandler_H

#include "XBee.h"
#include <SoftwareSerial.h>

enum LedType {STATUSLED = 0, ERRORLED, DATALED};

class XBeeHandler {
 public:
  explicit XBeeHandler(uint8_t statusLed, uint8_t errorLed, uint8_t dataLed, uint8_t rxPin, uint8_t txPin);
  void begin();
  void update();

 private:
  boolean CheckPackets();
  boolean DoCommand(XBeeResponse &resp);
  void HandleZBRxResponse(XBeeResponse &resp);
  void HandleModemStatusResponse(XBeeResponse &resp);
  void HandleRemoteAtCommandResponse(XBeeResponse &resp);
  void HandleAtCommandResponse(XBeeResponse &resp);
  void flashLed(LedType led, int times, int wait);

  SoftwareSerial *_serial;
  uint8_t _statusLed;
  uint8_t _errorLed;
  uint8_t _dataLed;

  XBee           _xbee;
  XBeeAddress64  _addr;

  ZBTxRequest            _zbTxReq;
  RemoteAtCommandRequest _atTxReq;

  XBeeResponse            _response;
  ZBTxStatusResponse      _zbTxStatusResp;
  ZBRxResponse            _zbRxResp;
  ModemStatusResponse     _modemStatusResp;
  RemoteAtCommandResponse _remoteAtCommandResp;
  AtCommandResponse       _atCommandResp;
};

#endif
