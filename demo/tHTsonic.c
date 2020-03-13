#include "rcx2.h"

/* Illustrates HITechnic Ultrasonic sensor */

int f;
task main()
{  int i;
	f=0;
	Show(f);
	SetSensorTypeC(SENSOR_1, SENSOR_TYPE_REFLECTION);
	SetSensorMode(SENSOR_1, SENSOR_MODE_PCT_FULLSCALE);
// 0 is less than about 8", 100 is more than about 4', +1 equals about 1/2 inch
	ClearSensorValueC(SENSOR_1);
// wait for object to come close
	while (true) {
		f=Sensor1;
		Wait(25);
		if (f==0) {break;}
	}
// wait for object to be removed
	while (true) {
		f=Sensor1;
		Wait(25);
		if (f==100) {break;}
	}
	f=9999;            Wait(200);
}
