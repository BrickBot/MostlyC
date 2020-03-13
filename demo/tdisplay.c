#include "rcx2.h"
#define HYPOT
#include "math.h"

/* Illustrates use of the console display; connect touch sensor to Port 1 */

float result;  /*must be a global variable for Show to work correctly*/
task main()
{	int i;
	SelectDisplay(DISPLAY_SENSOR_1); /*display RCX sensor 1 for 30 seconds*/
                                         /*the RAW values of the sensor are displayed*/
	Wait(30*100);
	PlaySystemSoundC(SOUND_ERROR);
	i=10;
	Show(result);  //displays hypotenuse of right triangle with side i on RCX window
	for (;--i>=0;) {
		result=hypoti(i,i);
		Wait(200);
		PlaySystemSoundC(SOUND_ERROR);
	}
	i=10;
	ViewSourceValue(1,result);  //displays value with correct decimal point
	for (;--i>=0;) {
		result=hypoti(i,i);
		Wait(200);
		PlaySystemSoundC(SOUND_ERROR);
	}
}
