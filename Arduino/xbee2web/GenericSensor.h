// TODO(riccardo) change name in the copyright boilerplate :D
// Copyright (c) 2015 Riccardo Miccini, Culo. All rights reserved.

#ifndef GenericSensor_H
#define GenericSensor_H

#include "SoftwareSerial.h"

// TODO(riccardo) add sensors and values types: just give them names
enum SensorType	{DALLAS_DS18B20 = 0, OTHER_SENSOR, ETC_ETC};
enum ValueType	{TEMPERATURE = 0, PRESSURE, CO2, LIGHT};

class GenericSensor {
public:
	GenericSensor(SensorType _sensor_type, uint8_t* _pin_settings);
	int16_t readValue(ValueType value_type);

	bool DEBUG_GENERIC_SENSOR;


private:
	void init();
	// TODO(riccardo) add reading functions for each sensor and value combination
	void init_Dallas_DS18B20();	// example
	// TODO(riccardo) add reading functions for each sensor and value combination
	int16_t read_Dallas_DS18B20_Temperature();	// example

	SensorType	sensor_type;
	uint8_t*	pin_settings;

	SoftwareSerial *serial;
};

#endif
