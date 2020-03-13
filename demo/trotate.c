#include "rcx2.h"

/* Illustrates rotation sensor */

int f;
task main()
{  int i;
	f=0;
	Show(f);
	SetSensorTypeC(SENSOR_1, SENSOR_TYPE_ROTATION);
	SetSensorMode(SENSOR_1, SENSOR_MODE_ANGLE);
// counts every 1/10 (i think) revolution forward, backward subtracts and can go negative
	ClearSensorValueC(SENSOR_1);
// wait for 3 positive revolutions
	while (true) {
		f=Sensor1;
		Wait(25);
		if (f>30) {break;}
	}
	f=9999;            Wait(200);
}
