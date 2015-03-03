// Copyright (c) 2015 Riccardo Miccini. All rights reserved.

#ifndef XBeeHandler_H
#define XBeeHandler_H

#include "XBee.h"
#include <SoftwareSerial.h>

#define MAX_PAYLOAD 100

enum LedType {STATUSLED = 0, ERRORLED, DATALED};
enum NodeType {COORDINATOR = 0, ROUTER, END_DEVICE};
enum DebugType {DEBUG_NONE = 0, DEBUG_SERIAL, DEBUG_LEDS, DEBUG_BOTH};

struct XBeeNode {
  uint16_t      addr16;
  XBeeAddress64 addr;
  String        id;
  uint16_t      addrParent;
  NodeType      type;
};

class XBeeHandler {
 public:
  XBeeHandler(uint8_t statusLed, uint8_t errorLed, uint8_t dataLed, uint8_t rxPin, uint8_t txPin);
  XBeeHandler(uint8_t statusLed, uint8_t errorLed, uint8_t dataLed);
  XBeeHandler(uint8_t rxPin, uint8_t txPin);
  XBeeHandler();
  ~XBeeHandler();

  void selectDebug(DebugType type);
  void setDebugLeds(uint8_t statusLed, uint8_t errorLed, uint8_t dataLed);
  void setDebugSerial(uint8_t rxPin, uint8_t txPin);
  void detachDebugLeds();
  void detachDebugLSerial();

  void begin();
  void update();
  uint8_t discover(XBeeNode list[], uint8_t maxNodes);


 private:
  boolean CheckPackets(uint16_t time);
  boolean CheckPacketsOld();
  void HandleResponse(XBeeResponse &resp);
  void HandleZBRxResponse(XBeeResponse &resp);
  void HandleModemStatusResponse(XBeeResponse &resp);
  void HandleRemoteAtCommandResponse(XBeeResponse &resp);
  void HandleAtCommandResponse(XBeeResponse &resp);
  void HandleUnknown(XBeeResponse &resp);
  void flashLed(LedType type, int times, int wait);
  void init();


  SoftwareSerial *_serial;
  uint8_t _statusLed;
  uint8_t _errorLed;
  uint8_t _dataLed;
  boolean _debugSerial;
  boolean _debugLeds;

  XBee           _xbee;
  XBeeAddress64  _addr;

  uint8_t       *_data;
  uint16_t       _dataLen;
  boolean        _dataEmpty;

  ZBTxRequest             _zbTxReq;
  RemoteAtCommandRequest  _remoteAtCommandReq;
  AtCommandRequest        _atCommandReq;

  XBeeResponse            _response;
  ZBTxStatusResponse      _zbTxStatusResp;
  ZBRxResponse            _zbRxResp;
  ModemStatusResponse     _modemStatusResp;
  RemoteAtCommandResponse _remoteAtCommandResp;
  AtCommandResponse       _atCommandResp;
};

#endif
