// Copyright (c) 2015 Riccardo Miccini, Abud Shoka. All rights reserved.


# include "GenericSensor.h"

// TODO(riccardo) add relevant libraries (ex Dallas etc)
#include "DallasTemperature.h"
#include "OneWire.h"
#include "DHT.h"
#include "math.h"


OneWire *one_wire;
DallasTemperature *dallas_sens;
DHT *dht;
NTC *ntc;



GenericSensor::GenericSensor(SensorType _sensor_type, uint8_t* _pin_settings) {
	sensor_type = _sensor_type;
	pin_settings = _pin_settings;
	DEBUG_GENERIC_SENSOR = true;
	init();
}

GenericSensor::~GenericSensor() {
	delete dallas_sens;
	delete one_wire;
	delete dht;

}

bool GenericSensor::readValue(ValueType value_type, int16_t* val) {
	switch (sensor_type) {
	case DALLAS_DS18B20:
		if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read integer from Dallas DS18B20 sensor");
		break;

	case AM2302:
		if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read integer from AM2302 sensor");
		break;

	case NTC:
	    if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read integer from NTC sensor");
		break;

	case OTHER_SENSOR:
		switch (value_type) {
		case TEMPERATURE:
			// do something
			break;
		case PRESSURE:
			// do something
			break;
		default:
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read from the sensor");
		}
		break;

	default:
		if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read from the sensor");
	}

	return false;
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


	case OTHER_SENSOR:
		switch (value_type) {
		case TEMPERATURE:
			// do something
			break;
		case PRESSURE:
			// do something
			break;
		default:
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read from the sensor");
		}
		break;

	default:
		if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't initialize the sensor");
	}
	
	return false;
}

void GenericSensor::init() {
	switch (sensor_type) {
	case DALLAS_DS18B20:
		init_Dallas_DS18B20();
		break;
	case AM2302:
		init_AM2302();
		break;
	case NTC:
	    init_NTC();
		break;
	case OTHER_SENSOR:
		// do something
		break;
	default:
		if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't enable the sensor");
	}
}


// TODO(riccardo) add reading functions for each sensor and value combination
void GenericSensor::init_Dallas_DS18B20(){
	one_wire = new OneWire(pin_settings[0]);
	dallas_sens = new DallasTemperature(one_wire);
	dallas_sens->begin();
}

void GenericSensor::init_AM2302(){
	dht = new DHT(pin_settings[0], DHT22);
	dht->begin();
}

void GenericSensor::init_NTC(){
	ntc = new NTC(pin_settings[0], NTC);
	ntc->begin();
}



// TODO(riccardo) add reading functions for each sensor and value combination
float GenericSensor::read_Dallas_DS18B20_Temperature(){
	dallas_sens->requestTemperatures();
	return dallas_sens->getTempCByIndex(0);
}

float GenericSensor::read_AM2302_Temperature(){
	return dht->readTemperature();
}

float GenericSensor::read_AM2302_Humidity(){
	return dht->readHumidity();
}

float GenericSensor::read_NTC_Temperature(){
 
 float val = analogRead(A0);
   ntc = log(((10230000/val) - 10000)); 
   ntc = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * ntc * ntc ))* ntc);
   ntc = ntc - 273.15;
	return ntc->readTemperature();
}
