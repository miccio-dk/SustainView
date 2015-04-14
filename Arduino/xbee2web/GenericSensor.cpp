// TODO(riccardo) change name in the copyright boilerplate :D
// Copyright (c) 2015 Riccardo Miccini, Culo. All rights reserved.


# include "GenericSensor.h"

// TODO(riccardo) add relevant libraries (ex Dallas etc)
#include "DallasTemperature.h"
#include "OneWire.h"


GenericSensor::GenericSensor(SensorType _sensor_type, uint8_t* _pin_settings) {
	sensor_type = _sensor_type;
	pin_settings = _pin_settings;
	DEBUG_GENERIC_SENSOR = true;
	init();
}

int16_t GenericSensor::readValue(ValueType value_type) {
	switch (sensor_type) {
	case DALLAS_DS18B20:
		switch (value_type) {
		case TEMPERATURE:
			return read_Dallas_DS18B20_Temperature();
			break;
		case PRESSURE:
			// do something
			break;
		default:
			if(DEBUG_GENERIC_SENSOR) serial->println("GenericSensor error: couldn't read from the sensor");
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

}

int16_t GenericSensor::read_Dallas_DS18B20_Temperature(){

}