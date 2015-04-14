// Copyright (c) 2015 Riccardo Miccini. All rights reserved.

#ifndef GenericSensor_H
#define GenericSensor_H

// TODO(riccardo) add relevant libraries (ex Dallas etc)
#include ".h"

#define MAX_PAYLOAD 100

// TODO(riccardo) add sensors and values types: just give them names
enum SensorType	{SENSOR_NAME = 0, SENSOR_NAME_2, ETC_ETC};
enum ValueType	{TEMPERATURE = 0, PRESSURE, CO2, LIGHT};


class GenericSensor {
public:
	GenericSensor(SensorType _sensor_type, uint8_t* _pin_settings);
	int16_t readValue(ValueType _value_type);


 private:
	void init();
	// TODO(riccardo) add reading functions for each sensor and value combination
	void init_Dallas();	// example
	// TODO(riccardo) add reading functions for each sensor and value combination
	int16_t read_Dallas_Temperature();	// example

	SensorType	sensor_type;
	ValueType	value_type;
	uint8_t*	pin_settings;
};

#endif
