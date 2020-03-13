#include "rcx2.h"

/* Uses motors/sound/touch to illustrate tasks */
/* A task can be created for any independent activity */

task touch() {
	int f;
	SetSensorTypeC(SENSOR_1, SENSOR_TYPE_SWITCH);
	SetSensorMode(SENSOR_1, SENSOR_MODE_RAW);
	while (true) {
// wait for switch to be pressed
		while (true) {
			f=Sensor1;
			if (f<500) {break;}
		}
		GoC(MOTOR_A);
// wait for switch to be released
		while (true) {
			f=Sensor1;
			if (f>500) {Wait(50); break;}
		}
		StopC(MOTOR_A);
	}
}
task sound() {
	while (true) {
		PlayToneC(2400,200);
		PlayToneC(3800,200);
		Wait(400);
	}
}
int f;
task main()
{  

	f=1111;   Show(f);
	start touch;
	start sound;
	Wait(1500);
	stop sound;
	Wait(700);
	stop touch;
	f=9999;            Wait(200);
}
