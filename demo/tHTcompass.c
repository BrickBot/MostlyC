#include "rcx2.h"

/* Illustrates HITechnic compass sensor */

int f;
task main()
{  int i;
	f=0;
	Show(f);
	SetSensorTypeC(SENSOR_1, SENSOR_TYPE_REFLECTION);
	SetSensorMode(SENSOR_1, SENSOR_MODE_PCT_FULLSCALE);
// 0 is north, 25 east, 50 south, 75 west, +1=3.6 degrees
// note 100 or 99 are also pretty close to north
	ClearSensorValueC(SENSOR_1);
// wait for compass to point exactly north
	while (true) {
		f=Sensor1;
		Wait(25);
		if (f==0) {break;}
	}
// wait for compass to point exactly south
	while (true) {
		f=Sensor1;
		Wait(25);
		if (f==50) {break;}
	}
	f=9999;            Wait(200);
}
