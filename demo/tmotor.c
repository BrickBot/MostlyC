#include "rcx2.h"

/* Illustrates turning motors on and off */
/* Motors can be turned on/off separately from power/direction */
/* Coast/Float leaves the axle free to turn but stops the motor */
/* Power vevels are 0 to 7, 0 turns very slowly */

int f;
task main()
{  
	f=8;   Show(f);    GoC(MOTOR_A);
	for (;--f>=0;) {
		SetPower(MOTOR_A,f);
		Wait(200);
	}
	PlaySystemSoundC(SOUND_FAST_UP);  Wait(200);
	SetPower(MOTOR_A,7);
	ForwardC(MOTOR_A); Wait(400);
	StopC(MOTOR_A);    Wait(200);
	BackC(MOTOR_A);    Wait(400);
	CoastC(MOTOR_A);   Wait(200);
	f=9999;            Wait(200);
}
