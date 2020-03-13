#include "rcx2.h"

/* Illustrates RCX reflection sensor */
/* Usually needs to be close to surface unless pointed at a light source */
/* Turn off when not in use to save battery */

int f;
task main()
{  int i;
	f=0;
	Show(f);
	SetSensorTypeC(SENSOR_1, SENSOR_TYPE_REFLECTION);
	SetSensorMode(SENSOR_1, SENSOR_MODE_RAW); /*0 to 1000*/
// pointing to a light source or reflective surface should be lower
	while (true) {
		f=Sensor1;
		Wait(50);
		if (f<700) {break;} /*wait for brightness*/
	}
	SetSensorMode(SENSOR_1, SENSOR_MODE_PCT_FULLSCALE); /*0 to 100*/
	/*reflective or bright is higher*/
	while (true) {
		f=Sensor1;
		Wait(50);
		if ((f>=40)&&(f<46)) {break;} //put thumb over light
	}
	SetSensorTypeC(SENSOR_1, SENSOR_TYPE_SWITCH); //turn light off
	f=9999;            Wait(200);
}
