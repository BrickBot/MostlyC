#include "rcx2.h"

/* Illustrates timer operation */

int f;
task main()
{  int i;
	f=0;
	ViewSourceValue(1,f);
	SetTimer0(100); /* 100 1/10s of a second */
	while (true) {
		f=Timer0; /* counts up from starting value */
		if (f>250) {break;} /* 15 seconds */
	}
	ViewSourceValue(0,f);
	Set10msTimer3(1000); /* 1000 1/100s of a second */
	while (true) {
		f=Timer10ms3; /* counts up from starting value */
		if (f>2500) {break;} /* 15 seconds */
	}
	f=9999;            Wait(200);
}
