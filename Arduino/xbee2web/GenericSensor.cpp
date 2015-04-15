// TODO(riccardo) change name in the copyright boilerplate :D
// Copyright (c) 2015 Riccardo Miccini, Culo. All rights reserved.


# include "GenericSensor.h"

// TODO(riccardo) add relevant libraries (ex Dallas etc)
#include "DallasTemperature.h"
#include "OneWire.h"

OneWire *one_wire;
DallasTemperature *dallas_sens;


GenericSensor::GenericSensor(SensorType _sensor_type, uint8_t* _pin_settings) {
	sensor_type = _sensor_type;
	pin_settings = _pin_settings;
	DEBUG_GENERIC_SENSOR = true;
	init();
}

GenericSensor::~GenericSensor() {
	delete dallas_sens;
	delete one_wire;
}

bool GenericSensor::readValue(ValueType value_type, int16_t* val) {
	switch (sensor_type) {
	case DALLAS_DS18B20:
		if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read integer from Dallas DS18B20 sensor");
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
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read value type from the sensor");
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
	case OTHER_SENSOR:
		// do something
		break;
	default:
		if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't enable the sensor");
	}
}

void GenericSensor::init_Dallas_DS18B20(){
	one_wire = new OneWire(pin_settings[0]);
	dallas_sens = new DallasTemperature(one_wire);
	dallas_sens->begin();
}

float GenericSensor::read_Dallas_DS18B20_Temperature(){
	dallas_sens->requestTemperatures();
	return dallas_sens->getTempCByIndex(0);
}