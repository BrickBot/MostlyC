#include "rcx2.h"

/* Illustrates temperature sensor */

int f;
task main()
{  int i;
	f=0;
	ViewSourceValue(1,f);
	SetSensorTypeC(SENSOR_1, SENSOR_TYPE_TEMPERATURE);
	SetSensorMode(SENSOR_1, SENSOR_MODE_FAHRENHEIT);
// show temperature 0 to 212 degrees accurate to a 1/10 degree
// hold the silver tip with both fingers until it heats up to 90 degrees
	while (true) {
		f=Sensor1;
		Wait(50);
		if (f>900) {break;}
	}
	SetSensorMode(SENSOR_1, SENSOR_MODE_CELSIUS);
// show temperature 0 to 100 degrees accurate to a 1/10 degree
// don't touch the silver tip until it cools off to 30C=9/5(30)+32=86F degrees
	while (true) {
		f=Sensor1;
		Wait(50);
		if (f<=300) {break;}
	}
	f=9999;            Wait(200);
}
