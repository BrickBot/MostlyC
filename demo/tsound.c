#include "rcx2.h"

/* Illustrates playing sounds */
/* Note that sounds are played asynchronously;
   that is, program keeps running as soon as
   the sound is queued.  So without the Wait
   in the loop, you won't hear 30 tones.
   Frequency is displayed in Hertz.
*/

int f;
task main()
{  int i;
	PlaySystemSoundC(SOUND_CLICK); Wait(100);
	PlaySystemSoundC(SOUND_DOUBLE_BEEP); Wait(100);
	PlaySystemSoundC(SOUND_DOWN); Wait(100);
	PlaySystemSoundC(SOUND_UP); Wait(100);
	PlaySystemSoundC(SOUND_UP); Wait(100);
	PlaySystemSoundC(SOUND_FAST_UP); Wait(100);
	PlayToneC(8000,200);
	i=30;  Show(f);
	for (;--i>=0;) {f=1000+i*100; PlayTone(f,100);  Wait(200);}
	f=9999; Wait(200);
}
