// Copyright (c) 2015 Riccardo Miccini, Abud Shoka. All rights reserved.


# include "GenericSensor.h"

#include "DallasTemperature.h"
#include "OneWire.h"
#include "DHT.h"
#include "math.h"
#include "Adafruit_MPL115A2.h"
#include "Wire.h"
#include "Adafruit_BMP085.h"


OneWire *one_wire;
void *obj;


GenericSensor::GenericSensor(SensorType _sensor_type, uint8_t* _pin_settings) {
	sensor_type = _sensor_type;
	pin_settings = _pin_settings;
	DEBUG_GENERIC_SENSOR = false;
}

GenericSensor::~GenericSensor() {
	delete one_wire;
	delete obj;
}

bool GenericSensor::readValue(ValueType value_type, float* val) {
	switch (sensor_type) {
	case DALLAS_DS18B20:
		if (value_type == TEMPERATURE) {
			*val = read_Dallas_DS18B20_Temperature();
			return true;
		} else {
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read value type from Dallas DS18B20 sensor");
		}
		break;

	case AM2302:
		if (value_type == TEMPERATURE) {
			*val = read_AM2302_Temperature();
			return true;
		} else if (value_type == HUMIDITY) {
			*val = read_AM2302_Humidity();
			return true;
		} else {
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read value type from AM2302 sensor");
		}
		break;

	case NTC:
	    if (value_type == TEMPERATURE) {
			*val = read_NTC_Temperature();
			return true;
		} else {
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read value type from NTC sensor");
		}
		break;

	case LDR:
	    if (value_type == LIGHT) {
			*val = read_LDR_Value();
			return true;
		} else {
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read value type from LDR sensor");
		}
        break;
	    
	case MPL115A2:
		if (value_type == TEMPERATURE) {
			*val = read_MPL115A2_Temperature();
			return true;
		} else if (value_type == PRESSURE) {
			*val = read_MPL115A2_Pressure();
			return true;
		} else {
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read value type from MPL115A2 sensor");
		}
		break;

	case BMP085:
		if (value_type == TEMPERATURE) {
			*val = read_BMP085_Temperature();
			return true;
		} else if (value_type == PRESSURE) {
			*val = read_BMP085_Pressure();
			return true;
		} else if (value_type == ALTITUDE) {
			*val = read_BMP085_Altitude();
			return true;
		} else {
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read value type from MPL115A2 sensor");
		}
		break;

	default:
		if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't initialize the sensor");
	}
	
	return false;
}

void GenericSensor::enableDebug(Stream &_serial) {
	serial = &_serial;
	DEBUG_GENERIC_SENSOR = true;
}

void GenericSensor::disableDebug() {
	DEBUG_GENERIC_SENSOR = false;
}


void GenericSensor::init() {
	switch (sensor_type) {
	case DALLAS_DS18B20:
		init_Dallas_DS18B20();
		break;

	case AM2302:
		init_AM2302();
		break;

	case MPL115A2:
	    init_MPL115A2();
	    break;

   case BMP085:
	    init_BMP085();
	    break;

	case NTC:
	case LDR:
		break;

	default:
		if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't enable the sensor");
	}
}

void GenericSensor::init_Dallas_DS18B20(){
	
}

void GenericSensor::init_AM2302(){
	
}

void GenericSensor::init_MPL115A2(){
	obj = new Adafruit_MPL115A2();
	static_cast<Adafruit_MPL115A2 *>(obj)->begin();
}

void GenericSensor::init_BMP085(){
	obj = new Adafruit_BMP085();
	static_cast<Adafruit_BMP085 *>(obj)->begin();
}

float GenericSensor::read_Dallas_DS18B20_Temperature(){
	// belongs to init funct
	one_wire = new OneWire(pin_settings[0]);
	obj = new DallasTemperature(&(*one_wire));
	static_cast<DallasTemperature *>(obj)->begin();

	static_cast<DallasTemperature *>(obj)->requestTemperatures();
	float a = static_cast<DallasTemperature *>(obj)->getTempCByIndex(0);

	delete obj;
	delete one_wire;

	return a;
}

float GenericSensor::read_AM2302_Temperature(){
	return static_cast<DHT *>(obj)->readTemperature();
}

float GenericSensor::read_AM2302_Humidity(){
	// belongs to init funct
	obj = new DHT(pin_settings[0], DHT22);
	static_cast<DHT *>(obj)->begin();

	float a = static_cast<DHT *>(obj)->readHumidity();

	delete obj;

	return a;
}

float GenericSensor::read_NTC_Temperature(){
	uint16_t val = analogRead(pin_settings[0]);
	float ntc = log(((10230000/val) - 10000)); 
	ntc = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * ntc * ntc )) * ntc);
	ntc = ntc - 273.15;
	return ntc;
}

float GenericSensor::read_LDR_Value(){
	//uint16_t val = analogRead(pin_settings[0]);
	//return (val<pin_settings[1]);
	uint16_t val = analogRead(pin_settings[0]);
	float lux = (2500.0/(val/1023.0*5)-500)/10;
	return lux;
}

float GenericSensor::read_MPL115A2_Temperature(){
	return static_cast<Adafruit_MPL115A2 *>(obj)->getTemperature();
}

float GenericSensor::read_MPL115A2_Pressure(){
	return static_cast<Adafruit_MPL115A2 *>(obj)->getPressure();
}

float GenericSensor::read_BMP085_Temperature(){
	return static_cast<Adafruit_BMP085 *>(obj)->readTemperature();
}

float GenericSensor::read_BMP085_Pressure(){
	return static_cast<Adafruit_BMP085 *>(obj)->readPressure();
}

float GenericSensor::read_BMP085_Altitude(){
	return static_cast<Adafruit_BMP085 *>(obj)->readAltitude();
}