#include "rcx2.h"

/* Illustrates HITechnic remote control sensor */

int f;
task main()
{  int i;
	f=0;
	Show(f);
	SetSensorTypeC(SENSOR_1, SENSOR_TYPE_REFLECTION);
	SetSensorMode(SENSOR_1, SENSOR_MODE_PCT_FULLSCALE);
// 1OFF=10 1ON=20 2OFF=30 2ON=40 3OFF=50 3ON=60 4OFF=70 4ON=80
// 0 when no buttons are pressed
	ClearSensorValueC(SENSOR_1);
// wait for all buttons to be pressed in order starting with 1OFF
	for (i=1; i<=8; i++) {
		while (true) {
			f=Sensor1;
			Wait(25);
			if (f==(i*10)) {break;}
		}
	}
	f=9999;            Wait(200);
}
