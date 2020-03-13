#include "rcx2.h"

/* Uses motors/sound to illustrate tasks */
/* A task can be created for any independent activity */

task motor() {
	int i;
	GoC(MOTOR_A);
	i=8;
	for (;--i>=0;) {
		SetPower(MOTOR_A,i);
		Wait(100);
	}
	SetPower(MOTOR_A,7);
	ForwardC(MOTOR_A); Wait(400);
	StopC(MOTOR_A);    Wait(200);
	BackC(MOTOR_A);    Wait(400);
	CoastC(MOTOR_A);   Wait(200);
}
task sound() {
	while (true) {
		PlayToneC(1400,200);
		PlayToneC(1800,200);
		Wait(400);
	}
}
int f;
task main()
{  

	f=1111;   Show(f);
	start motor;
	start sound;
	Wait(1500);
	stop sound;
	Wait(700);
	stop motor;
	f=9999;            Wait(200);
}
