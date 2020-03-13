// math.h : one digit of precision math functions.
//
#ifndef _MATH_
#define _MATH_
#ifdef WIN32
#define casti(x) ((int)((x)*10.0))
#define castf(x) (((float)(x))/10.0)
#endif
#ifdef ASINCOS
#ifndef ATAN
#define ATAN
#endif
#ifndef SQRT
#define SQRT
#endif
#define asinf(x) castf(math(16,casti(x)) )
#define acosf(x) castf(math(17,casti(x)) )
#endif
#ifdef HYPOT
#ifndef SQRT
#define SQRT
#endif
#define hypoti(x,y) castf(math(13,((x)<<8)|y) )
#endif
#ifdef SQRT
#define sqrti(x) math(1,(x))
#define sqrtf(x) castf(math(10,casti(x)) )
#endif
#ifdef LOG
#define logf(x) castf(math(7,casti(x)) )
#define log10f(x) castf(math(8,casti(x)) )
#endif
#ifdef EXP
#define expf(x) castf(math(12,casti(x)) )
#endif
#define CONST_E 2.7183  //2.7183
#define CONST_PI 3.14159 //3.14159
#ifdef TANGENT
#ifndef COSINE
#define COSINE
#endif
#define tanf(x) castf(math(4,casti(x)) )
#endif
#ifdef COSINE
#ifndef SINE
#define SINE
#endif
#define cosf(x) castf(math(3,casti(x)) )
#endif
#ifdef SINE
#define sinf(x) castf(math(2,casti(x)) )
#endif

#define radianTodegree(x) (float(x)*57.2958)     //only good to 5.7 due to overflow
#define degreeToradian(x) (float(x)*1.745/100.0) //only good to 180 due to overflow
#ifdef ATAN
#define atanf(x) castf(math(15,casti(x)) )
#endif


int math(short op, short r) {
   union {short t,next;};
#if defined(SQRT)
   short b,c;
#endif
#if defined(TANGENT)||defined(ASINCOS)||defined(HYPOT)
   short d,e;   //used for tan; all vars together otherwise overlap with temps
#endif
   next=0;
   while (true) { 
     if (op<=0) {return (r);}
#if defined(SINE)
	 if (op==2) { //sin  radians>=0
		op=next;
		while (r>63) {r-=63;}
		if (r<=0) {r=0; continue;}
		if (r<=5) {continue;}
		if (r<=7) {r=6; continue;}
		if (r<=8) {r=7; continue;}
		if (r<=10) {r=8; continue;}
		if (r<=12) {r=9; continue;}
		if (r<=18) {r=10; continue;}
		if (r<=21) {r=9; continue;}
		if (r<=23) {r=8; continue;}
		if (r<=34) {r=31-r; continue;}
		if (r<=40) {r=32-r; continue;}
		if (r<=41) {r=-8; continue;}
		if (r<=44) {r=-9; continue;}
		if (r<=50) {r=-10; continue;}
		if (r<=52) {r=-9; continue;}
		if (r<=54) {r=-8; continue;}
		r -= 63; continue;
	 } //sin
#endif
#ifdef COSINE
	 if (op==3) { //cos
		r+=16;  //pi/2
		op=2; continue;
	 }
#endif
#ifdef SQRT
     if (op==1) { //sqrti
		op=next;
		if (r<0) {r=-9999; continue;}
		if (r==0) {continue;}
		if (r>32700) {r=181; continue;}
		c=0; b=0x4000;
		while (true) {
			t = c;
			t += b;
			c /= 2;
			if (t <= r) {
				r -= t;
				c += b;
			}
			b /= 4;
			if (b==0) {break;}
		}
		r=c;
	 }  //sqrti
#endif
#ifdef TANGENT
	 if (op==4) { //tan1
		while (r>63) {r-=63;}
		op=0;
		if (r==45) {r=46; continue;}
		if (r==46) {r=89; continue;}
		if (r==47) {r=807; continue;}
		if (r==48) {r=-114; continue;}
		if (r==7)  {r=8; continue;}
		if (r==10) {r=16; continue;}
		if (r==11) {r=20; continue;}
		if (r==12) {r=26; continue;}
		if (r==13) {r=36; continue;}
		if (r==14) {r=58; continue;}
		if (r==15) {r=141; continue;}
		if (r==16) {r=-342; continue;}
		if (r==17) {r=-77; continue;}
		if (r==18) {r=-43; continue;}
		d=r;  op=2;  next=5;  continue;
	}
	if (op==5) { //tan2
		e=r;  r=d;  op=3;  next=6;  continue;
	}
	if (op==6) { //tan3
		op=0; r=casti( castf(e)/castf(r) ); continue; //sin(x)/cos(x)
	}
#endif
#ifdef SQRT
	 if (op==10) { //sqrtf
		if (r<3277) {r*=10; op=1; next=0; continue;}
		r=(r+5)/10; op=1; next=11; continue;
	 }
     if (op==11) {op=0; r*=10; continue;} //sqrtf
#endif
#ifdef LOG
	 if (op==7) { //ln
        op=next;
		if (r<=0) {r = -9999; continue;}
		if (r>32000) {r = 81; continue;}
		t=0;
		while (r>20) {r/=2; t+=693;}
		t+=(r-10)*70;
		r=(t+50)/100;
		continue;
	 } //ln
     if (op==8) { //log10-a
		op=7;  next=9;  continue;
	 } //log10-a
     if (op==9) { //log10-b
		op=0; r=((r*100)/23+5)/10; continue;  //log(x)==ln(x)/ln(10)
	 } //log10-b
#endif
#ifdef EXP
	 if (op==12) { //exp  
		op=0;
		t=0;
		if (r<0) {
			if (r<=-32) {r=0; continue;}
			r=-r; t=1;
		}
		if (r>=81) {r=32767;}
		else if (r==80) {r=29810;}
		else if (r>=70) {r=10967+(r-70)*(1778);}
		else if (r>=60) {r=4034+(r-60)*(654);}
		else if (r>=50) {r=1484+(r-50)*(241);}
		else if (r>=40) {r=546+(r-40)*(89);}
		else if (r>=30) {r=201+(r-30)*(33);}
		else if (r>=20) {r=74+(r-20)*(12);}
		else if (r>=10) {r=27+((r-10)*43)/10;}
		else {r=10+(r*16)/10;}
		if (t) {r=(1000/r+5)/10;}
		continue;
	 } //exp
#endif
#ifdef HYPOT
	 if (op==13) { //hypoti-a
		 t=r&0xff; r=(r>>8)&0xff;
		 if (r>t) {c=r; r=t; t=c;}
		 r=r*100/t;
		 r=r*r+10000; d=t;  op=1;  next=14;
	 } //hypoti-a
	 if (op==14) { //hypoti-b
		op=0;
		r=(r*d+5)/10;
	 }
#endif
#ifdef ATAN
	 if (op==15) { //atan
		 op=next;
		 t=abs(r);
		if (t>=480) {t=16;}
		else if (t>=86) {t=15;}
		else if (t>=46) {t=14;}
		else if (t>=31) {t=13;}
		else if (t>=23) {t=12;}
		else if (t>=18) {t=11;}
		else if (t>=14) {t=10;}
		else if (t>=12) {t=9;}
		else if (t>=10) {t=8;}
		else if (t>=8)  {t=7;}
		if (r<0) {t=-t;}
		r=t;
	 } //atan
#endif
#ifdef ASINCOS
	 if (op==16) { //asin
		 op=0;
		 t=abs(r);
		if (t==10) {t=16;}
		else if (t==9) {t=11;}
		else if (t>=7) {t+=1;}
		if (r<0) {t=-t;}
		r=t;
	 } //asin
	 if (op==17) { //acos
		 if (r==0) {return (16);}
		 e=r;
		 r=1000/(r*r)-10;
		 r*=10;  op=1;  next=18;
	 } //acos
	 if (op==18) {op=15; next=19;}
	 if (op==19) {
		 op=0;
		 if (e<0) {r=31-r;}
	 }
#endif
   } //while
}

#endif //_MATH_
