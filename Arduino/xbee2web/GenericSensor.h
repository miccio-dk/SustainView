// Copyright (c) 2015 Riccardo Miccini, Abud Shoka. All rights reserved.

#ifndef GenericSensor_H
#define GenericSensor_H

#include "SoftwareSerial.h"

// TODO(riccardo) add sensors and values types: just give them names
enum SensorType	{DALLAS_DS18B20 = 0, AM2302, BMP085, MPL115A2, NTC, LDR OTHER_SENSOR, ETC_ETC};
enum ValueType	{TEMPERATURE = 0, HUMIDITY, PRESSURE, LIGHT};

class GenericSensor {
public:
	GenericSensor(SensorType _sensor_type, uint8_t* _pin_settings);
	~GenericSensor();
	bool readValue(ValueType value_type, int16_t* val);
	bool readValue(ValueType value_type, float* val);

	bool DEBUG_GENERIC_SENSOR;


private:
	void init();
		

	SensorType	sensor_type;
	uint8_t*	pin_settings;
	SoftwareSerial*	serial;

	void init_Dallas_DS18B20();
	void init_AM2302();
	void init_MPL115A2();

	float read_Dallas_DS18B20_Temperature();
	float read_AM2302_Temperature();
	float read_AM2302_Humidity();
	float read_NTC_Temperature();
	float read_LDR_Value();
	float read_MPL115A2_Temperature();
	float read_MPL115A2_Pressure();

};

#endif
