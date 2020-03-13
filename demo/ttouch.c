#include "rcx2.h"

/* Illustrates touch sensor */
/* open around >1000, pushed around <50 in raw mode */

int f;
task main()
{  int i;
	f=0;
	Show(f);
	SetSensorTypeC(SENSOR_1, SENSOR_TYPE_SWITCH);
	SetSensorMode(SENSOR_1, SENSOR_MODE_RAW);
// wait for switch to be pressed
	while (true) {
		f=Sensor1;
		if (f<500) {break;}
	}
// wait for switch to be released
	while (true) {
		f=Sensor1;
		if (f>500) {Wait(50); break;}
	}
	SetSensorMode(SENSOR_1, SENSOR_MODE_BOOLEAN);
// wait for switch to be pressed; 0 open, 1 pressed
	while (true) {
		f=Sensor1;
		if (f) {break;}
	}
// wait for switch to be released
	while (true) {
		f=Sensor1;
		if (!f) {Wait(50); break;}
	}
	f=9999;            Wait(200);
}
