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

#include "SoftwareSerial.h"
#include <XBee.h>
#include <Ethernet.h>
#include <SPI.h>
#include "RestClient.h"

#define PL_NODE_POS		5
#define PL_NODE			(0<<PL_NODE_POS)

#define NIMBITS_ADDR	"192.168.137.133"
#define NIMBITS_PORT	8080
#define NIMBITS_PATH	"/service/v2/value"			// "series" doens't work locally - what a shame
#define NIMBITS_EMAIL	"riccardo.miccini@gmail.com"
#define NIMBITS_TOKEN	"test_tkn"
#define NIMBITS_FULLPATH	NIMBITS_PATH "?email=" NIMBITS_EMAIL "&token=" NIMBITS_TOKEN "&id=" NIMBITS_EMAIL "/node"	// remove from id on for "series"

#define SOFTRX		A2
#define SOFTTX		A3
#define STATUSLED	13
#define ERRORLED	13

byte mac[] = { 0x12, 0x12, 0x19, 0x93, 0xBE, 0xEF };
IPAddress _dns(8,8,8,8);
IPAddress ip(192,168,137,50);

typedef struct Reading_T {
	char *type;
	float value;
};
char *value_types[] = {"temp", "hum", "press", "light", "alt"};
Reading_T readings[5];
Reading_T xb_standalone;
uint8_t node = 0;

SoftwareSerial serialSW(SOFTRX, SOFTTX);
RestClient client = RestClient(NIMBITS_ADDR, NIMBITS_PORT);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0xFFFF);
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();

void flashLed(int pin, int times, int wait);
float unpackFloat(uint8_t *c);
float LDRtoFloat(uint16_t val);
uint8_t parsePayload(ZBRxResponse rx, uint8_t *node, Reading_T *readings);
uint16_t uploadData(uint8_t node, Reading_T *readings, uint8_t readings_num);

void setup() {
	pinMode(STATUSLED, OUTPUT);
	pinMode(ERRORLED, OUTPUT);

	serialSW.begin(9600);
	serialSW.println("Starting up..");
	Serial.begin(9600);
	xbee.setSerial(Serial);
 	Ethernet.begin(mac, ip, _dns);
	serialSW.println("- setup done");
}


void loop() {
	xbee.readPacket();

	if(xbee.getResponse().isAvailable()) {
		serialSW.println("\nReceived packet!");

		if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
//			serialSW.println("- it's a RX message");
			xbee.getResponse().getZBRxResponse(rx);

			if(rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
//				serialSW.println("- it's been acknowledged");
				flashLed(STATUSLED, 5, 10);
			} else {
				serialSW.println("- packet not acknowledged");
				flashLed(ERRORLED, 2, 20);
			}

			// timestamp?
			uint8_t result = parsePayload(rx, &node, readings);
			if(result) {
				uint16_t status = uploadData(node, readings, result);
				if(status == 200) {
//					serialSW.println("- pushed to server");
					flashLed(STATUSLED, 5, 10);
				} else {
					serialSW.println("- error pushing to server");
					flashLed(ERRORLED, 2, 20);
				}
			}
		} else if(xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
			xbee.getResponse().getZBRxIoSampleResponse(ioSample);
			if(ioSample.isAnalogEnabled(1)) {
				xb_standalone.type = value_types[3];
				xb_standalone.value = LDRtoFloat(ioSample.getAnalog(1));
				serialSW.print("- found ");
				serialSW.println(ioSample.getAnalog(1), DEC);

				uint16_t status = uploadData(4, &xb_standalone, 1);
				if(status == 200) {
	//				serialSW.println("- pushed to server");
					flashLed(STATUSLED, 5, 10);
				} else {
					serialSW.println("- error pushing to server");
					flashLed(ERRORLED, 2, 20);
				}
			}
		} else {
			serialSW.print("- it's an unknown packet. API Id: ");
			serialSW.println(xbee.getResponse().getApiId());
			flashLed(ERRORLED, 2, 50);
		}
	} else if(xbee.getResponse().isError()) {
		serialSW.print("- it's an error. Code: ");  
		serialSW.println(xbee.getResponse().getErrorCode());
	}
}


void flashLed(int pin, int times, int wait) {
	for(int i = 0; i < times; i++) {
		digitalWrite(pin, HIGH);
		delay(wait);
		digitalWrite(pin, LOW);
		if (i+1 < times)	delay(wait);
	}
}

float unpackFloat(uint8_t *c) {
	float f;
	memcpy(&f, c, 4);
	return f;
}

float LDRtoFloat(uint16_t val) {
	float lux = (2500.0/(val/1023.0*3.3)-500)/10;
	return lux;
}

uint8_t parsePayload(ZBRxResponse rx, uint8_t *node, Reading_T *readings) {
	uint8_t payload_len = rx.getDataLength();
	uint8_t payload_info = rx.getData(0);
	uint8_t *payload = (rx.getData()+1);
	*node = payload_info>>PL_NODE_POS;
	uint8_t est_readings_num = (payload_len-1)/4;
	uint8_t readings_num = 0;

	serialSW.print("- node number: ");
	serialSW.println(*node);

	for(int i=0; i<5; i++) {
		if((payload_info&(1<<i)) > 0) {
			readings[readings_num].type = value_types[i];
			readings[readings_num].value = unpackFloat(payload+(readings_num*4));
			serialSW.print("- found ");
			serialSW.print(readings[readings_num].type);
			serialSW.print(" = ");
			serialSW.println(readings[readings_num].value, 2);
			readings_num++;
		}
	}

	if(readings_num == est_readings_num) {
		return readings_num;
	} else {
		serialSW.println("- error! readings don't match payload size");
		return 0;
	}
}

uint16_t uploadDataSeries(uint8_t node, Reading_T *readings, uint8_t readings_num) {
	char buff_val[10];
	char buff_str[100];
	memset(buff_str, 0, 100);
	char body[256];
	memset(body, 0, 256);
	body[0] = '[';

	serialSW.print("- generating json.. ");
	for(int i=0; i<readings_num; i++) {
		strcat(buff_str, "{\"key\":\"");
		strcat(buff_str, NIMBITS_EMAIL);
		strcat(buff_str, "/node");
		utoa(node, buff_val, 10);	
		strcat(buff_str, buff_val);
		strcat(buff_str, "_");
		strcat(buff_str, readings[i].type);
		strcat(buff_str, "\",\"values\":[{\"d\":\"");
		dtostrf(readings[i].value, 4, 3, buff_val);
		strcat(buff_str, buff_val);
		strcat(buff_str, (i==(readings_num-1) ? "\"}]}" : "\"}]},"));
		strcat(body, buff_str);
		strcpy(buff_str, "");
	}
	strcat(body, "]");

	serialSW.println(body);
	uint16_t res = client.post(NIMBITS_FULLPATH, body);
	serialSW.print("- upload result: ");
	serialSW.println(res);

	return res;
}

uint16_t uploadData(uint8_t node, Reading_T *readings, uint8_t readings_num) {
	uint16_t res;
	char buff_val[10];
	char path[128];
	char body[128];
	memset(path, 0, 128);
	memset(body, 0, 128);
	
	for(int i=0; i<readings_num; i++) {
		strcpy(path, NIMBITS_FULLPATH);
		utoa(node, buff_val, 10);	
		strcat(path, buff_val);
		strcat(path, "_");
		strcat(path, readings[i].type);

		strcpy(body, "{\"d\":\"");
		dtostrf(readings[i].value, 4, 3, buff_val);
		strcat(body, buff_val);
		strcat(body, "\"}");

		serialSW.print("- sending");
		serialSW.print(body);
		serialSW.print("\tto ");
		serialSW.print(path);
		serialSW.print("... ");
		res = client.post(path, body);
		serialSW.println(res);
	}
	return res;
}

int freeRam() {
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}