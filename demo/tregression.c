//#define TEST_TASK		/* 202 bytes free */
//#define TEST_LOCALS		/* 920 bytes free */
#define TEST_PARAMETERS	/* 86 bytes free */
//#define TEST_GLOBALS		/* 207 bytes free */

#include "rcx2.h"
int error;
int xx[4];
float yy[4];
#if defined(TEST_TASK)||defined(TEST_GLOBALS)
bool zz[4];
#endif
const char ss[]="abC";

#if defined(TEST_TASK)||defined(TEST_GLOBALS)
void err(int number) {
	if (number==0) {return;}
	error=number+9000;
	PlaySystemSoundC(SOUND_DOUBLE_BEEP);
	Wait(1000);
}

int test(int a, int b, int c, bool d, float e) {
	if (a==1) {
		if ((b!='j')||(c!=34)||(!d)||(e!=4.5)) {return(72);}
	} else if (a==2) {
		if ((b!=49)||(c!=7)||d||(e!=9.0)) {return(73);}
	} else if (a==3) {
		if ((b!=3)||(c!=30)||(!d)||(e!=81.0)) {return(74);}
	} else {return(78);}
	return (0);
}

	float f,g,h;
#if defined(TEST_GLOBALS)
	bool b,c;  int i,j,k;
#endif
task main() {
#if defined(TEST_TASK)
	bool b,c;  int i,j,k;
#endif
#else
#define err(n) if (n) {error=n+9000; PlaySystemSoundC(1); Wait(1000);}
#if defined(TEST_PARAMETERS)
void sub(float f, float g, float h, bool b, bool c, int i, int j, int k) {
#else
void sub() {
	float f,g,h;
	bool b,c;  int i,j,k;
#endif
#endif
	error=9999; Show(error);
#if defined(TEST_PARAMETERS)
	if ((f!=1.0)||(g!=2.0)||(h!=3.0)||(!b)||c||(i!=4)||(j!=5)||(k!=6)) {err(79);}
#endif
	b=true;  i=999;   f=123.4;
	if (false) {err(1);}
	if (0) {err(2);}
	if (1) {i++;} else {err(3);}
	if (b) {i++;} else {err(4);}
	if (i!=1001) {err(5);}
	if (0x3e9==i) {i++;} else {err(6);}
	if (0X3E9>i) {err(7);}
	if (1003>i) {i++;} else {err(8);}
	if (1002<i) {i--;} else {err(9);}
	if (i<=1001) {err(10);}
	if (i>=1003) {err(11);}
	if (0.0) {err(12);}
	if (1.1) {f++;} else {err(13);}
	if (f) {f++;} else {err(14);}
	if (f!=1.254e2) {err(15);}
	if (125.4==f) {f++;} else {err(16);}
	if (126.4>f) {err(17);}
	if (1270.0e-1>f) {f++;} else {err(18);}
	if (1270.0E-1<f) {f--;} else {err(19);}
	if (f<=125.4) {err(20);}
	if (f>=127.4) {err(21);}
	b=true; c=false; i=1; j=2; f=1.0; g=2.0;
	if (b&c) {err(22);}
	if (!b) {err(23);}
	if (b|c) {j+=2;} else {err(24);}
	if (b^b) {err(25);}
	if (b&&c) {err(26);}
	if (b||c) {j-=1;} else {err(27);}
	if ((i+j)!=4) {err(28);}
	if ((i-j)!=-2) {err(29);}
	if ((i*j)!=3) {err(30);}
	if ((j/i)!=3) {err(31);}
	if ((j%2)!=1) {err(32);}
	if ((i&j)!=1) {err(33);}
	if ((i|j)!=3) {err(34);}
	if ((-j)>=0) {err(35);}
	if ((~i)!=-2) {err(36);}
	if ((j**2)!=011) {err(37);}
	if ((j<<3)!=24) {err(76);}
	if (1!=(j>>1)) {err(77);}
	if ((f+g)!=3.0f) {err(38);}
	if ((f-g)!=-1.0) {err(39);}
	if ((f*g)!=2.0) {err(40);}
	if ((g/f)!=2.0) {err(41);}
	if (f&&g) {g-=2.0;} else{err(43);}
	if (g||g) {err(44);}
	if ((-f)>=0.0) {err(45);} else {f+=0.5;}
	if ((f**2.0)!=2.2) {err(46);}
	b=true; c=false; i=1; j=2; f=1.0; g=2.0;
	b&=(b|c);
	c|=b;
	if (!(b&c)) {err(47);}
	i+=j;
	i*=3;
	i-=(j-1);
	i/=j;
	if (i!=4) {err(48);}
	j%=3;
	if (j!=2) {err(49);}
	i|=3;;
	if (i!=7) {err(50);}
	i&=j;
	if (i!=2) {err(51);}
	f+=g;
	f*=3.0;
	f-=(g-1.0);
	f/=g;
	if (f!=4.0) {err(52);}
	while (b) {i++; break; err(53);}
	while (!b) {err(54);}
	i=3; while (i) {if (i==3) {i=0; continue;} err(55);}
	f=3.0; while (f) {if (f==3.0) {f=0.0; continue;} err(56);}
	while (b&&c) {b=false;  f++;}
	if (f!=1.0) {err(57);}
	i=(b)?9:10;
	if (i!=10) {err(58);}
	i=3; j=0; for (;--i>=0;) {j+=i;}
	if (j!=3) {err(59);}
	f=3.0; g=0.5; for (;--f>=0;) {g+=f;}
	if (g!=3.5) {err(60);}
#if defined(TEST_TASK)||defined(TEST_GLOBALS)
	const int x[]={1,2,3,4,5};
	const float y[]={1.1,2.2,3.3,4.4,5.5};
	const bool z[]={true,false,true,false,true};
	i=5; b=true; for (;--i>=0;) {
		if (x[i]!=(i+1)) {err(61);}
		f=y[i];  g=(float(i+1)*1.1); 
		if (f!=g) {err(62);}
		if (z[i]!=b) {err(63);}
		b=!b;
	}
#endif
	if ((ss[0]!='a')||(ss[2]!='C')) {err(75);}
	if (rand()<0) {err(64);}
	i=-5;  j=3;
	i=abs(i)+sign(i)+max(i,j)+min(-8,j);
	if (i!=-1) {err(65);}
	f=-5.0;  g=3.0;
	f=abs(f)+sign(f)+max(f,g)+min(-8.0,g);
	if (f!=-1.0) {err(66);}
	i=4;
	for (;--i>=0;) {
	  xx[i]=i;  yy[i]=float(i);
#if defined(TEST_TASK)||defined(TEST_GLOBALS)
	  zz[i]=bool(i&1);
#endif
	}
	if ((xx[0]!=0)||(xx[3]!=3)||(yy[0]!=0.0)||(yy[3]!=3.0)
#if defined(TEST_TASK)||defined(TEST_GLOBALS)
	||(!zz[3])
#endif
	) {err(67);}
	i=4;  j=3; f=5.0;  g=4.0; c=false;
	k=i++ + j--;
	if ((k!=7)||(i!=5)||(j!=2)) {err(68);}
	k=--i + ++j;
	if ((k!=7)||(i!=4)||(j!=3)) {err(69);}
	h=f++ + g--;
	if ((h!=9.0)||(f!=6.0)||(g!=3.0)) {err(70);}
	h=--f + ++g;
	if ((h!=9.0)||(f!=5.0)||(g!=4.0)) {err(71);}
	xx[0]=1; xx[1]=2; yy[0]=1.0; yy[1]=2.0; i=0; j=1;
	xx[0]+=xx[j];
	xx[i]*=3;
	xx[i]-=(j+i);
	xx[i]/=xx[1];
	if (xx[i]!=4) {err(80);}
	xx[1]%=3;
	if (xx[j]!=2) {err(81);}
	xx[i]|=3;;
	if (xx[i]!=7) {err(82);}
	xx[i]&=xx[j];
	if (xx[i]!=2) {err(83);}
	yy[0]+=yy[1];
	yy[i]*=3.0;
	yy[i]-=(yy[j]-1.0);
	yy[i]/=yy[j];
	if (yy[i]!=4.0) {err(84);}
	xx[0]=4;  xx[1]=3; yy[0]=5.0;  yy[1]=4.0;
	k=xx[0]++ + xx[j]--;
	if ((k!=7)||(xx[0]!=5)||(xx[1]!=2)) {err(85);}
	k=--xx[i] + ++xx[j];
	if ((k!=7)||(xx[0]!=4)||(xx[1]!=3)) {err(86);}
	h=yy[0]++ + yy[j]--;
	if ((h!=9.0)||(yy[0]!=6.0)||(yy[1]!=3.0)) {err(87);}
	h=--yy[i] + ++yy[j];
	if ((h!=9.0)||(yy[i]!=5.0)||(yy[j]!=4.0)) {err(88);}
#if defined(TEST_TASK)||defined(TEST_GLOBALS)
	err(test(1,'j',34,true,4.5));
	err(test(2,k*k,k,c,h));
	i=3;
	err(test(3,xx[i],xx[3]*10,k>1,h*h));
#endif
	PlaySystemSoundC(SOUND_DOUBLE_BEEP);PlaySystemSoundC(SOUND_DOUBLE_BEEP);PlaySystemSoundC(SOUND_DOUBLE_BEEP);
}
#if defined(TEST_PARAMETERS)
int m,n;
int arr(int i[], float f[]) {
	m=0; n=3;
	if ( (i[0]!=0)||(i[3]!=3)||(f[0]!=0.0)||(f[3]!=3.0)) {err(89);}
	if ( (i[m]!=0)||(i[n]!=3)||(f[m]!=0.0)||(f[n]!=3.0)) {err(90);}
	i[1]=4;  f[1]=4.0;  i[2]=5;  f[2]=5.0;
	i[m]=3;  f[m]=3.0;  i[n]=6;  f[n]=6.0;
	return (999);
}
task main() {
	sub(1.,2.,0.3e1,true,false,4,5,6);
	m=4; for(;--m>=0;) {xx[m]=m; yy[m]=float(m);}
	if (arr(xx,yy)!=999) {err(91);}
	m=4; for(;--m>=0;) {
		if (xx[m]!=(m+3)) {err(92);}
		if (yy[m]!=(float(m)+3.0)) {err(93);}
	}
}
#endif

#if defined(TEST_LOCALS)
task main() {
	sub();
}
#endif
