#include "rcx2.h"
/*Since space is precious, the math functions must be enabled individually*/
#define SQRT
#define HYPOT
#define SINE
#define COSINE
#define TANGENT
#define ASINCOS
#define ATAN
#define LOG
#define EXP
#include "math.h"

int error;
void err(int number) {
	if (number==0) {return;}
	Show(error);
	error=number+9000;
	PlaySystemSoundC(SOUND_DOUBLE_BEEP);
	Wait(1000);
}

float a,b;
task main()
{	
	int x, y;
	x=5; y=-6;
// abs(x), min(x,y), max(x,y), rand(), sign(x)
	if (abs(x)<=0) {err(1);}
	if (abs(x)!=5) {err(2);}
	if (abs(y)!=6) {err(3);}
	if (min(x,y)!=-6) {err(4);}
	if (max(x,y)!=5) {err(5);}
	if (rand()==999) {err(6);}  /*possible, but low probability*/
	if (sign(x)!=1) {err(7);}
	if (sign(y)!=-1) {err(8);}
// float(x)  int(a)
	a=float(x);
	if (a!=5.0) {err(9);}
	a=5.9;
	x=int(a);
	if (x!=5) {err(10);}
// sqrt(x)  hypot(x,y)
	x=1024;  a=130.6;
	if (sqrti(x)!=32) {err(11);}
	a=sqrtf(a);
	if (a!=11.4) {err(12);}
	if (hypoti(3,4)!=5.0) {err(13);} /*integer args, but float result*/
// e   pi
	a=CONST_E; /*compiler does constant expressions to a higher precision so must
                        assign to a variable to get an accurate comparison*/
	if (a!=2.7) {err(14);}
	a=CONST_PI;
	if (a!=3.1) {err(15);}
// sinf(a)  cosf(a)  tanf(a)
	a=1.2;
	if (sinf(a)!=0.9) {err(16);}
	a=1.2;
	if (cosf(a)!=0.3) {err(17);}
	a=1.2;
	if (tanf(a)!=2.6) {err(18);}
// asinf(a)  acosf(a)  atanf(a)
	b=1.2;
	a=asinf(b);
	if (a!=1.3) {err(19);}
	a=acosf(b);
	if (a!=-1.6) {err(20);}
	a=atanf(b);
	if (a!=0.9) {err(21);}
// logf(a)  log10f(a)  exp(a)
	a=100.0;
	if (log10f(a)!=2.0) {err(22);}
	if (logf(a)!=4.5) {err(23);}
	a=3.0;
	b=expf(a);
	if (b!=20.1) {err(24);}
	err(999);	
}
