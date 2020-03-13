#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef UNDER_CE
#include <assert.h>
#else
#include <windows.h>
#define assert(x) {if (!(x)) MessageBox(NULL,L"error",L"error",MB_OK);}
#define system(x) 0
#define stricmp _stricmp
#define itoa _itoa
#endif
#include <math.h>
#ifdef WIN32
#define strcasecmp stricmp
#define strncasecmp strnicmp
#define strcasestr strstr
#endif

#if !defined(_MTYPE_H)
#define _MTYPE_H
#include <stdio.h>


#define VAL_LIMIT 128
typedef char Name[37];  //35 limit + @ + NULL
typedef struct _Context {
	int nNames;
	int lineno;
	char **intfs;
	int intf;
	int cons;
	int type;
	int task;
	unsigned char subs[512];
	char val[VAL_LIMIT];
	Name names[512];
	Name lasmName[512];
} Context;

#define C_VOID 0
#define C_STRING 1
#define C_INTEGER 2
#define C_ARRAY 3
#define C_VARIABLE 4
#define C_FLOAT 5
#define C_BOOL 6
#define C_RESULT 7

#define POWER 'M'
#define MULTIPLY 'N'
#define DIVIDE 'O'
#define MOD 'P'
#define INVERT 'R'
#define ADD 'S'
#define SUBTRACT 'T'
#define AND 'U'
#define ANDAND 'V'
#define OR 'W'
#define OROR 'X'
#define EQL 'Y'
#define NEQL 'Z'
#define GTR 'a'
#define GEQ 'b'
#define LSS 'c'
#define LEQ 'd'
#define INTDIV 'e'
#define SHIFTR '0'
#define SHIFTL '1'
#define UNARYPLUS '2'
#define FLIP '3'
#define XOR '4'
#define NOT '5'

typedef struct _Output {
	int nArgs;
	char opcode[12];
	union {
		struct {char arg1[12],arg2[12],arg3[12],arg4[12],arg5[12],arg6[12];};
		char line[72];
	};
	} Output;
typedef struct _Outputs {
	int count;
	Output code[6000];
} Outputs;
typedef char SCons[128];
SCons arrays[16];
int arrayg[16];
int arrayn[16];
int arrayt[16];
int arraycnt=0;
bool needMask=false;
Outputs code;
static int error(int code);
static int error2(int code, char s[]);
static int error3(int code, int arg);
static void emit(char opcode[], char arg1[],char arg2[],char arg3[],char arg4[],char arg5[],char arg6[]) {
	code.code[code.count].nArgs=0;
	strcpy(&code.code[code.count].opcode[0],opcode);
	strcpy(&code.code[code.count].arg1[0],&arg1[0]);
	strcpy(&code.code[code.count].arg2[0],&arg2[0]);
	strcpy(&code.code[code.count].arg3[0],&arg3[0]);
	strcpy(&code.code[code.count].arg4[0],&arg4[0]);
	strcpy(&code.code[code.count].arg5[0],&arg5[0]);
	strcpy(&code.code[code.count].arg6[0],&arg6[0]);
	if (arg1[0]) code.code[code.count].nArgs++;
	if (arg2[0]) code.code[code.count].nArgs++;
	if (arg3[0]) code.code[code.count].nArgs++;
	if (arg4[0]) code.code[code.count].nArgs++;
	if (arg5[0]) code.code[code.count].nArgs++;
	if (arg6[0]) code.code[code.count].nArgs++;
	code.count++;
}
static void EMITline(char *s) {
	code.code[code.count].nArgs=-1;
	strcpy(code.code[code.count].line,s);
	code.count++;
}
#define UNEMIT() {code.count--;}
#define EMIT(op) emit(op,"","","","","","");
#define EMIT1(op,arg1) emit(op,arg1,"","","","","")
#define EMIT2(op,arg1,arg2) emit(op,arg1,arg2,"","","","")
#define EMIT3(op,arg1,arg2,arg3) emit(op,arg1,arg2,arg3,"","","")
#define EMIT4(op,arg1,arg2,arg3,arg4) emit(op,arg1,arg2,arg3,arg4,"","")
#define EMIT6(op,arg1,arg2,arg3,arg4,arg5,arg6) emit(op,arg1,arg2,arg3,arg4,arg5,arg6)
#endif //_MTYPE_H
#define M_FAIL -1
#define M_NOERR 0
typedef char NM[4];
static NM tasks[10]={"Z0","Z1","Z2","Z3","Z4","Z5","Z6","Z7","Z8","Z9"};
static NM subs[8]={"P0","P1","P2","P3","P4","P5","P6","P7"};
static NM globals[32]={"G0","G1","G2","G3","G4","G5","G6","G7","G8",
"G9","G10","G11","G12","G13","G14","G15","G16","G17","G18","G19","G20","G21","G22","G23","G24",
"G25","G26","G27","G28","G29","G30","G31"};
static NM temps[16]={"$0","$1","$2","$3","$4","$5","$6","$7","$8",
"$9","$10","$11","$12","$13","$14","$15"};
static int tempcnt=1,tempmask=1;
static int globalcnt=1,globalmask=1;
static int labelCnt=1;
static int subCnt=0;
static int taskCnt=0;
static int maxLocal=0;
static int maxTask=0;
static int gbool=0,lbool=0;
static int unioning=-1;
static char tref[8],smax[8]={0};

static int getLabel() {return labelCnt++;}
static char *makeLabel(int i) {
	char *s=new char[8];
	assert(s!=NULL);
	sprintf(s,"s%d",i);
	return s;
}
static char *findName(Context *c, char *n) {
	int i,j=c->nNames; Name s;
	strcpy(s,n);
	if (c->task>=0) strcat(s,"@");
	for (i=0; i<j; i++) if (strcasecmp(s,c->names[i])==0) return c->lasmName[i]; //local
	if (c->task<0) return NULL;
	for (i=0; i<j; i++) if (strcasecmp(n,c->names[i])==0) return c->lasmName[i]; //global
	return NULL;
}
static int defineName(Context *c, char *n, int type);
static char * getName(Context *c, char *n) {
	int i,j=c->nNames;
	for (i=0; i<j; i++) if (strcasecmp(n,c->names[i])==0) return c->lasmName[i];
	i=defineName(c,n,0);
	return c->lasmName[i];
}
static char *getTempN(Context *c, int n=1) {
	int i,j,k;
	if (unioning>0) i=unioning;
	else {
		k=(1<<n)-1;
		for (i=0; i<tempcnt; i++)
			if (((k<<i)&tempmask)==0) {
				break;
			}
		tempmask|=(k<<i);
		if (i>=tempcnt) {
			tempcnt+=n;
			if (tempcnt>=16) {error(106); return NULL;}
		}
	}
	j=c->nNames++;
	if ((n>1)&&(c->task==0)) i+=n-1;
	strcpy(c->names[j],temps[i]);
	strcpy(c->lasmName[j],c->names[j]);
	c->lasmName[j][0]=(c->task==0)?'T':'L';
	EMIT3(";define",c->names[j],"as",c->lasmName[j]);
	assert(temps[i][0]=='$');
	if (unioning==0) unioning=i;
	return temps[i];
}
#define getTemp(x) getTempN(x)
static void freeTemp(Context *c, char *name) {
	int i=atoi(name+1);
	assert(name[0]=='$');
	assert(i<tempcnt);
	assert(tempmask&(1<<i));
	tempmask &= ~(1<<i); 
}
static int isTemp(char *name) { return name[0]=='$';}
static char *getGlobal(Context *c, int n) {
	int i,j;
	j=(1<<n)-1;
	if (globalmask!=((1<<globalcnt)-1)) {
		assert(n==1);
		for (i=0; i<globalcnt; i++)
			if (((1<<i)&globalmask)==0) {
				return globals[i];
			}
		assert(0);
	}
	globalmask|=(j<<globalcnt); globalcnt+=n;
	if (globalcnt>=32) {error(107); return NULL;}
	return globals[globalcnt-n];
}
static int freeName(Context *c, char *name) {
	int i,j=c->nNames;
	for (i=0; i<j; i++) if (strcasecmp(name,c->names[i])==0) {
		c->names[i][0]=0;
		return 0;
	}
	return 1;
}
static void freeLocals(Context *c) {
	int i,j=c->nNames;
	for (i=0; i<j; i++) 
		if ((c->lasmName[i][0]=='T')||(c->lasmName[i][0]=='L')) c->names[i][0]=0;
}
static int isArray(char *name) {return name[1]=='A';}
static int isMatrix(char *name) {return (name[1]=='M')||(name[1]=='Y');}
static int isSpecial(char *name) {return (name[0]=='R')||(name[0]=='#');}
static int isParameterMatrix(char *name) {return name[1]=='Y';}
static int isGlobal(char *name) { return name[0]=='G';}
static int isLocal(char *name) { return (name[0]=='L')||(name[0]=='T');}
static int isWritable(char *name) {return (!isArray(name))&&(isGlobal(name)||isLocal(name));}
static int isProcedure(char *name) { return name[0]=='P';}
static int isTask(char *name) { return name[0]=='Z';}
static int isBool(char *name) {return (name[1]=='B')||(name[1]=='X');}
static int isBoolParameter(char *name) {return name[1]=='X';}
static int isFloat(char *name) {return name[1]=='F';}
static int isInteger(char *name) {return !isBool(name)&&!isFloat(name);}
static int defineName(Context *c, char *n, int type) {
	int g,i,j,k; char *p,temp[8];
	g=(c->task<0)&&(n[0]!='$');
	i=c->nNames++;
	strcpy(c->names[i],n);
	if (c->task>=0) strcat(c->names[i],"@");
	if (type>99) {
		j=type%100;
		type=type/100;
		k=type==99999;
		if ((j==1)||(type==99999)) type=1;
		if (g) {
			p=getGlobal(c,type);
			if (p==NULL) return 0;
			strcpy(temp,p);
		} else {
			p=getTempN(c,type);
			if (p==NULL) return 0;
			strcpy(temp,p);
			temp[0]=(c->task==0)?'T':'L';
			UNEMIT();
		}
		temp[5]=0; temp[4]=temp[2]; temp[3]=temp[1];
		temp[1]=k?'Y':'M';
		c->cons=C_ARRAY;
		c->type=(j==0)?C_INTEGER:(j==1)?C_BOOL:C_FLOAT;
		temp[2]=(j==0)?'I':(j==1)?'B':'F';
		strcpy(c->lasmName[i],temp);
		EMIT3(";define",n,"as",c->lasmName[i]);
		return 1;
	}
	if ((type==4)||(type==5)||(type==6)) {
		if (g) temp[0]='G';
		else temp[0]=(c->task==0)?'T':'L';
		temp[1]='A';
		itoa(arraycnt-1,temp+2,10);
		strcpy(c->lasmName[i],temp);
		c->cons=C_ARRAY;
		c->type=(type==4)?C_INTEGER:(type==5)?C_FLOAT:C_BOOL;;
		arrayg[arraycnt-1]=0;
		arrayt[arraycnt-1]=c->type;
		EMIT3(";define",n,"as",c->lasmName[i]);
		return 1;
	}
	if (type==1) {
		temp[1]='B';
		if (g) {temp[0]='G'; j=gbool++;}
		else {temp[0]=(c->task==0)?'T':'L'; j=lbool++;}
		itoa(j,temp+2,10);
		assert(j<=14);
		strcpy(c->lasmName[i],temp);
	} else if (g) {
		p=getGlobal(c,1);
		if (p==NULL) return 0;
	} else {
		p=getTemp(c);
		if (p==NULL) return 0;
		strcpy(temp,p);
		p=&temp[0];
		p[0]=(c->task==0)?'T':'L';
		UNEMIT();
	}
	if (type==0) strcpy(c->lasmName[i],p);
	c->cons=C_VARIABLE;
	c->type=C_INTEGER;
	if (type==1) c->type=C_BOOL;
	if (type==2) {
		c->type=C_FLOAT;
		c->lasmName[i][0]=p[0]; c->lasmName[i][1]='F';
		strcpy(&c->lasmName[i][2],p+1);
	}
	if (type==3) {
		c->type=C_BOOL;
		c->lasmName[i][0]=p[0]; c->lasmName[i][1]='X';
		strcpy(&c->lasmName[i][2],p+1);
	}
	EMIT3(";define",n,"as",c->lasmName[i]);
	return 1;
}
static void setBooleanBase(char name[], char out[], int *bit) {
	int i;
	i=atoi(name+2);
	*bit=i%100;
	if (isGlobal(name)) strcpy(out,"G");
	else strcpy(out,"L");
	itoa(i/100,out+1,10);
}
#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif
static int makeTask(Context *c, char *n, int index) {
	int i;
	for (i=0;i<8;i++) tref[i]=0;
	i=c->nNames++;
	strcpy(c->names[i],n);
	if (index>=10) {error(108); return 0;}
	if (lbool>=15) {error(109); return 0;}
	strcpy(c->lasmName[i],tasks[index]);
	EMIT3(";define",n,"as",c->lasmName[i]);
	EMIT1("task",c->lasmName[i]);
	return 1;
}
static int makeSub(Context *c, char *n) {
	int i;
	i=c->nNames++;
	strcpy(c->names[i],n);
	if (lbool>=15) {error(110); return 0;}
	if (subCnt++>=7) {error(111); return 0;}
	strcpy(c->lasmName[i],subs[subCnt-1]);
	EMIT3(";define",n,"as",c->lasmName[i]);
	EMIT1("sub",c->lasmName[i]);
	return 1;
}

static void setSub(Context *c, int g, int n, int r, int isFunc) {
	c->subs[subCnt*5-5]=isFunc; c->subs[subCnt*5-4]=r; c->subs[subCnt*5-3]=g&0xff;  c->subs[subCnt*5-2]=g>>8;  c->subs[subCnt*5-1]=n;
}

static int getSub(Context *c, int subIndex, int &g, int &r, int &n) {
	r=c->subs[subIndex*5+1]; g=c->subs[subIndex*5+2]|(c->subs[subIndex*5+3]<<8);  n=c->subs[subIndex*5+4];
	return c->subs[subIndex*5];
}

static int fromString(char *s, int *ii, Context *c) {
	int i,j,k,l,line=c->lineno; char ss[26],*ps;
	i=strspn(s+*ii," \n\r\t ");
	if (i>0) *ii+=i;
	if ((s[*ii]=='"')||(s[*ii]=='\''))
	{
		i=s[*ii];
		*ii+=1; l=1; c->val[0]='C';
		for (j=0;s[*ii];j++) {
			if (s[*ii]==i) {
				*ii+=1;  c->val[l]=0; c->cons=C_STRING; c->type=C_STRING;
				if (l==2) {c->cons=C_INTEGER; c->type=C_INTEGER; itoa(c->val[1],c->val+1,10); c->val[0]='N'; }
				return M_NOERR;
			} else if (s[*ii]=='\\') {
				*ii+=1;
				if (s[*ii]=='n') k='\n';
				else if (s[*ii]=='r') k='\r';
				else if (s[*ii]=='t') k='\t';
				else if (s[*ii]=='\\') k='\\';
				else if (s[*ii]=='\'') k='\'';
				else if (s[*ii]=='"') k='"';
				else if (s[*ii]=='0') {
					k=strspn(s+*ii,"01234567");
					strncpy(ss,s+*ii,k);
					ss[k]=0;  *ii+=k-1;
					sscanf(ss,"%lo",&k);
					if (k==0) break;
				}
				else break;
				c->val[l]=k;
			} else {
				c->val[l]=s[*ii];
				if (s[*ii]=='\n') (c->lineno)++;
			}
			*ii+=1; l++;
		}
		return error(112);
    }
	// Number
	k=0;
	if ((s[*ii]=='0')&&(toupper(s[*ii+1])=='X')) i=strspn(s+*ii,"0123456789abcdefABCDEFxX");
	else {
		i=0;
		if (s[*ii]=='+') *ii+=1;
		else if (s[*ii]=='-') i=1;
		i+=strspn(s+*ii+i,"0123456789.");
	}
	*ii+=i;
	if (i==0) return error(113);
	j=0;
	*ii=*ii-i-j-k;
	strncpy(ss,s+*ii,i+j+k);

	ss[i+j+k]=0;
	*ii=*ii+i+j+k;

	if ((ss[0]=='0')&&(ss[1]!='.')) {
		if (toupper(ss[1])!='X') {
			i=strspn(ss,"01234567");
			if (i!=(int)strlen(ss)) return error(114);
			sscanf(ss,"%lo",&k);
		} else sscanf(ss,"%lx",&k);
zz:		//if ((s[*ii]=='l')||(s[*ii]=='L')) *ii+=1;
		c->cons=C_INTEGER;  c->type=C_INTEGER; c->val[0]='N'; strcpy(&c->val[1],itoa(k,ss,10));
	} else if ((ps=strchr(ss,'.'))==NULL) {
		sscanf(ss, "%ld", &k); goto zz;
	} else {
		if (strchr(ps+1,'.')) return error(115);
		if ((s[*ii]=='e')||(s[*ii]=='E')) {
			i=*ii;   *ii+=1;
			if ((s[*ii]=='+')||(s[*ii]=='-')) *ii+=1;
			j=strspn(s+*ii,"0123456789"); *ii+=j;
			if (j==0) return error(116);
			strncat(ss,s+i,*ii-i);
		}
		if ((s[*ii]=='f')||(s[*ii]=='F')) *ii+=1;
		c->cons=C_FLOAT;  c->type=C_FLOAT; c->val[0]='F'; strcpy(&c->val[1],ss);
	}
	return M_NOERR;
}

static int expr(char *s, int *ii, int ls, Context *c);
static int state(char *s, int *ii, int ls, Context *lineno);
static int linestart=0;
static void L(char *s, int end) {
	char line[512];
	if (end<=linestart) return;
	line[0]=';';
	strncpy(line+1,s+linestart,end-linestart); line[end-linestart+1]=0;
	if ((end<=(linestart+2))&&(strspn(line+1," \t\n\r")==strlen(line+1)))
		return;
	EMITline(line);
}
static int W(char *s, int *ii, int ls, int *lineno) {
	for (;*ii<ls;) {
		//Ignore all of the folowing "\n\r\t " and space and move to next i
		while ((*ii<ls)&&((s[*ii]==' ')||(s[*ii]=='\t')||(s[*ii]=='\r')||(s[*ii]=='\n'))) {
			if (s[*ii]=='\n') {(*lineno)++; L(s,*ii); linestart=*ii+1;}
			*ii+=1;
		}
		//ignore comments starting with '//'
		if ((*ii<ls)&&(s[*ii]=='/')&&(s[*ii+1]=='/'))
		{
			while ((*ii<ls)&&(s[*ii]!='\n')&&(s[*ii]!='\r')) *ii+=1;
			continue;
		}
		if ((*ii<ls)&&(s[*ii]=='/')&&(s[*ii+1]=='*')) {
			*ii+=2;
			for (;;) {
				while ((*ii<ls)&&(s[*ii]!='\n')&&(s[*ii]!='*')) *ii+=1;
				if (s[*ii]=='\n') {(*lineno)++; L(s,*ii); linestart=*ii+1;}
				else if (s[*ii]!='*') return error(7);
				else if (s[(*ii)+1]=='/') {*ii+=2; break;}
				*ii+=1;
			}			
		} else break;
	}
	return 0;
}
static char *ftoa(float z,char *s,int x) {
	sprintf(s,"%g",z);
	return s;
}
static int pow2(char *s) {
	int i=atoi(s+1),j,k=1;
	assert(i>=0);
	for (j=0; j<=15; j++,k+=k)
		if (k==i) return j;
	return 0;
}
static int isNumber(int cons) {
	return (cons==C_INTEGER)||(cons==C_FLOAT);
}
static int isFloat(int cons) {
	return (cons==C_FLOAT);
}
static int isInteger(int cons) {
	return (cons==C_INTEGER);
}
static int isBool(int cons) {
	return (cons==C_BOOL);
}
static int iscConstant(Context *c) {
	return isNumber(c->cons)||isBool(c->cons);
}
static bool iscConstantTrue(Context *c) {
	float f=(float)atof(c->val+1);
	return f!=0.0f;
}
static int isVariable(int cons) {
	return (cons==C_VARIABLE);
}
static int isReadable(Context *c) {
	if (c->cons!=C_VOID) return true;
	return false;
}
static int iscBool(Context *c) {
	if (c->cons==C_BOOL) return true;
	if (( (c->cons==C_VARIABLE)||(c->cons==C_RESULT))&&(c->type==C_BOOL)) return true;
	return false;
}
static int iscBoolParameter(Context *c) {
	if (c->cons==C_BOOL) return false;
	if ((c->cons==C_VARIABLE)&&(c->type==C_BOOL)&&(c->val[1]=='X')) return true;
	return false;
}
static int iscInteger(Context *c) {
	if (c->cons==C_INTEGER) return true;
	if (( (c->cons==C_VARIABLE)||(c->cons==C_RESULT))&&(c->type==C_INTEGER)) return true;
	return false;
}
static int iscFloat(Context *c) {
	if (c->cons==C_FLOAT) return true;
	if (( (c->cons==C_VARIABLE)||(c->cons==C_RESULT))&&(c->type==C_FLOAT)) return true;
	return false;
}
static void incs(char *s, int n) {
	float i=(float)atof(s+1);
	if (isInteger(s)) ftoa(i+1.0f*(float)n,s+1,10);
	else ftoa(i+0.1f*(float)n,s+1,10);
}
static void inc(Context *s, int n) { //inc to next higher/lower value to switch <= to <
	float i=(float)atof(s->val+1);
	if (isInteger(s->cons)) ftoa(i+1.0f*(float)n,s->val+1,10);
	else ftoa(i+0.1f*(float)n,s->val+1,10);
}
static int factorx(char *s, int *ii, int ls, Context *c,int *n);
static int factor(char *s, int *ii, int ls, Context *c) {
	int i,n;
	i=factorx(s,ii,ls,c,&n);
	//if ((n<-2)||(n==-1)) return M_FAIL;
	if (n<=0) return i;
	assert(0);//outint(out,index,n-1);
	//out[(*index)++]=ARRAY;
	return i;
}
static void getArray(int ai, int sub, char out[]) {
	char *s=arrays[ai],*t;
	for (;;) {
		t=strchr(s,',');
		sub--;
		if (sub<0) break;
		s=t+1;
	}
	if (t==NULL) strcpy(out,s);
	else {
		strncpy(out,s,t-s);
		out[t-s]=0;
	}
}
static int doProcedure(Context *c, char *s, int &i,int ls, int &f, char *js) {int j,k,m,r;
			char name[8];
			if (c->task!=0) return error(117);
			if (W(s,&i,ls,&c->lineno)) return M_FAIL;
			if (s[i++]!='(') return error(118);
			j=atoi(js+1);
			f=getSub(c,j,j,r,k);
			for (m=0;;m++) {int n;
				if (W(s,&i,ls,&c->lineno)) return M_FAIL;
				if (s[i]==')') {i++; break;}
				if (s[i]==',') {
					if (m==0) return error(119);
					i++;
				}
				n=expr(s,&i,ls,c);
				if (n!=M_NOERR) return n;
				name[0]='L';
				itoa(m+1,name+1,10);
				n=j&3;  j=j>>2;
				if (m>k) return error3(120,m+1);
				if (r&(1<<m)) {
					if (!isMatrix(c->val)) return error3(121,m+1);
					if ((n==0)&&(c->type!=C_INTEGER)) return error3(122,m+1);
					else if ((n==2)&&(c->type!=C_FLOAT)) return error3(123,m+1);
					strcpy(c->val+1,c->val+3);
					EMIT2("lea",name,c->val);
					continue;
				}
				if ((n==0)&&!iscInteger(c)) return error3(124,m+1);
				if ((n==2)&&!iscFloat(c)) return error3(125,m+1);
				if ((n==3)&&!iscBool(c)) return error3(126,m+1);
				if (isTemp(c->val)) {
					freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));
					EMIT2("setv",name,c->val);
				} else {
					if (iscBool(c)&&!iscConstant(c)) {char temp4[8],temp5[8]; int mm;
						setBooleanBase(c->val,temp5,&mm);
						temp4[0]='N'; itoa((1<<mm),temp4+1,10);
						EMIT2("setv",name,temp5);
						EMIT2("andv",name,temp4);
						EMIT2("divv",name,temp4);
					} else EMIT2("setv",name,c->val);
				}
			}
			if (m!=k) return error3(127,k+1);
			tref[atoi(js+1)]++;
			EMIT1("calls",js);
			return M_NOERR;
}
static int factorx(char *s, int *ii, int ls, Context *c,int *n)
{
	int failure=M_FAIL; char *js;
	int k,m,i,f,modifier=0;
	*n=0;
	if (W(s,ii,ls,&c->lineno)) return M_FAIL;
	i=*ii;
	if ((s[i]=='+')&&(s[i+1]=='+')) {modifier=1; i+=2; goto zz;}
	if ((s[i]=='-')&&(s[i+1]=='-')) {modifier=2; i+=2; goto zz;}
	//test the following and send to fromstring to interpret
	if ( (s[i]=='+')||(s[i]=='-')||(s[i]=='\'')||
		(s[i]=='"')||((s[i]>='0') && (s[i] <= '9')) )
	{
		failure=fromString(s,ii,c);
		return failure;
	}
zz:
	if (((s[i]>='a') && (s[i] <= 'z'))||((s[i]>='A') && (s[i] <= 'Z'))) { char name[36]; char *ps,*pps;
		m=strspn(s+i+1,"abcdefg_hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		strncpy(name,s+i++,m+1); name[m+1]=0;
		if (m>35) return error2(128,name);
		if (m>0) i+=m;
		*ii=i;
		if (strcmp(name,"true")==0) {
			if (modifier) return error(129);
			c->cons=C_BOOL; c->type=C_BOOL;
			strcpy(c->val,"N1");
			return M_NOERR;
		}
		if (strcmp(name,"false")==0) {
			if (modifier) return error(130);
			c->cons=C_BOOL; c->type=C_BOOL;
			strcpy(c->val,"N0");
			return M_NOERR;
		}
		if (strcmp(name,"rand")==0) {
			if (modifier) return error(131);
			if (W(s,ii,ls,&c->lineno)) return M_FAIL;
			if (s[*ii]!='(') return error(132);
			*ii+=1;
			if (W(s,ii,ls,&c->lineno)) return M_FAIL;
			if (s[*ii]!=')') return error(133);
			*ii+=1;
			c->cons=C_RESULT; c->type=C_INTEGER;
			strcpy(c->val,"R0");
			return M_NOERR;
		}
		if (strcmp(name,"event")==0) {
			if (modifier) return M_FAIL;
			if (W(s,ii,ls,&c->lineno)) return M_FAIL;
			if (s[*ii]!='(') return error(134);
			*ii+=1;
			failure = expr (s,ii,ls,c);
			if ( failure!=M_NOERR ) return failure;
			if (c->cons!=C_INTEGER) return error(135);
			if (W(s,ii,ls,&c->lineno)) return M_FAIL;
			if (s[*ii]!=',') return error(136);
			*ii+=1;
			m=atoi(c->val+1);
			if ((m<0)||(m>5)) return error3(137,m);
			failure = expr (s,ii,ls,c);
			if ( failure!=M_NOERR ) return failure;
			if (c->cons!=C_INTEGER) return error(138);
			f=atoi(c->val+1);
			if ((f<0)||(f>15)) return error3(139,f);
			pps=getTemp(c);
			if (pps==NULL) return M_FAIL;
			strcpy(c->val,pps);
			pps=getName(c,c->val);
			if (s[*ii]!=')') return error(140);
			*ii+=1;
			sprintf(name,"E%2d,%2d",(m==0)?25:(m+26),f);
			EMIT2("setv",pps,name);
			c->cons=C_RESULT; c->type=C_INTEGER;
			return M_NOERR;
		}
		if ((strcmp(name,"abs")==0)||(strcmp(name,"sign")==0)||(strcmp(name,"int")==0)||(strcmp(name,"bool")==0)||
			(strcmp(name,"float")==0)||(strcmp(name,"castf")==0)||(strcmp(name,"casti")==0)) {
			if (modifier) return error2(141,name);
			if (W(s,ii,ls,&c->lineno)) return M_FAIL;
			if (s[*ii]!='(') return error2(142,name);
			failure = factorx (s,ii,ls,c,n);
			if ( failure!=M_NOERR ) return failure;
			if (isNumber(c->cons)) {float z;
				z=(float)atof(c->val+1);
				if (name[0]=='a') z=(z>0)?z:-z;
				if (name[0]=='b') {z=(z!=0)?1.0f:0.0f; c->val[0]='N'; c->type=C_BOOL; c->cons=C_BOOL;}
				if (name[0]=='i') {z=(float)(int)z; c->val[0]='N'; c->type=C_INTEGER; c->cons=C_INTEGER;}
				if (name[0]=='f') {c->val[0]='F'; c->type=C_FLOAT; c->cons=C_FLOAT;}
				if (name[0]=='s') z=(float)((z==0)?0: ((z>0)?1:-1));
				if (name[0]=='c') {
					return error2(143,name);
				}
				ftoa(z,c->val+1,10);
				return M_NOERR;
			}
			if (isTemp(c->val)) {
				ps=c->val;
				pps=getName(c,c->val);
				k=c->type;
				if (name[0]=='a') EMIT2("absv",pps,pps);
				else if (name[0]=='b') {
					c->type=C_BOOL;   goto bo;
				} else if (name[0]=='i') {
					c->type=C_INTEGER;
bo:					if (isInteger(k)||isBool(k)) return M_NOERR;
					if (!isFloat(k)) return error(144);
					EMIT2("divv",pps,"N10");
				} else if (name[0]=='f') {
					if (iscFloat(c)) return M_NOERR;
					if (!iscInteger(c)) return error(145);
					EMIT2("mulv",pps,"N10");
					c->type=C_FLOAT;
				} else if (name[0]=='c') {
					//m=(name[4]=='i')?C_FLOAT:C_INTEGER;
					//if (c->type!=m) return M_FAIL;
					c->type=(name[4]=='i')?C_INTEGER:C_FLOAT;
					return M_NOERR;
				} else EMIT2("sgnv",pps,pps);
			} else {
				ps=getTemp(c);
				if (ps==NULL) return M_FAIL;
				pps=getName(c,ps);
				k=c->type;
				if (name[0]=='a') EMIT2("absv",pps,c->val);
				else if (name[0]=='b') {
					c->type=C_BOOL;   goto boo;  //TODO absv( sgnv() )
				} else if (name[0]=='i') {
					c->type=C_INTEGER;
boo:				if (isInteger(k)||isBool(k)) return M_NOERR;
					if (!isFloat(k)) return error(144);
					EMIT2("setv",pps,c->val);
					EMIT2("divv",pps,"N10");
				} else if (name[0]=='c') {
					EMIT2("setv",pps,c->val);
					m=(name[4]=='i')?C_FLOAT:C_INTEGER;
					if (c->type!=m) return error(146);
					c->type=(name[4]=='i')?C_INTEGER:C_FLOAT;
				} else if (name[0]=='f') {
					if (iscFloat(c)) return M_NOERR;
					if (!iscInteger(c)) return error(145);
					EMIT2("setv",pps,c->val);
					EMIT2("mulv",pps,"N10");
					c->type=C_FLOAT;
				} else {
					EMIT2("sgnv",pps,c->val);
					if (iscFloat(c)) EMIT2("mulv",pps,"N10");
				}
			}
			c->cons=C_RESULT;
			strcpy(c->val,ps);
			return M_NOERR;
		}
		if ((js=findName(c,name))==NULL) return error2(147,name);
		if (isProcedure(js)) {
			if (modifier) return error2(148,name);
			i=*ii;
			failure=doProcedure(c,s,i,ls,f,js);
			*ii=i;
			if (failure!=M_NOERR) return failure;
			if (f==0) return error2(149,name);
arr:		js=getTemp(c);
			if (js==NULL) return M_FAIL;
			strcpy(name,getName(c,js));
			strcpy(c->val,js);
			c->cons=C_RESULT;
			if (f==1) {EMIT2("setv",name,"L1"); c->type=C_INTEGER;}
			if (f==2) {EMIT2("setv",name,"L1"); c->type=C_BOOL;}
			if (f==3) {EMIT2("setv",name,"LF1"); c->type=C_FLOAT;}
			return M_NOERR;
		} else {
			if (c->cons==C_VOID) {
				if (isMatrix(js)) {char *p,*q; int z;
					if (s[*ii]!='[') {
						if (modifier!=0) return error2(150,name);
						if (isParameterMatrix(js)) return error2(151,name);
						if (js[2]=='B') return error2(152,name);
						c->cons=C_ARRAY;
						c->type=(js[2]=='I')?C_INTEGER:C_FLOAT;
						strcpy(c->val,js);
						return M_NOERR;
					}
					*ii+=1;
					f=atoi(js+3);  m=js[2];  z=js[1];
					name[0]=js[0]; name[1]=0; strcat(name,js+3);
					k=expr(s, ii, ls, c);
					if (k!=M_NOERR) return M_FAIL;
					if (s[*ii]!=']') return error(153);
					*ii+=1;
					if (!isInteger(c->val)) return error(154);
					if (c->cons!=C_INTEGER) {
						if (isTemp(c->val)) {
							p=getName(c,c->val);
						} else {
							js=getTemp(c);
							if (js==NULL) return M_FAIL;
							p=getName(c,js);
							EMIT2("setv",p,c->val);
							strcpy(c->val,js);
						}
						if (z=='Y') goto moo;
						if (m=='B') {
							if (modifier!=0) return error(155);
							needMask=true;
							if (c->task!=0) return error(156);
							EMIT2("setv","L1","N-1");
							EMIT2("setv","L2",p);
							EMIT1("calls"," 7");
							EMIT2("setv",p,name);
							EMIT2("andv",p,"L1");
							EMIT2("divv",p,"L1");
						} else {
							EMIT2("addav",p,name);
							EMIT1("indv",p);
						}
mo:						c->cons=C_RESULT;
						c->type=(m=='I')?C_INTEGER:(m=='F')?C_FLOAT:C_BOOL;
						if (modifier) {char *j;
mooo:						UNEMIT();
							j=getTemp(c);
							if (j==NULL) return M_FAIL;
							q=getName(c,j);
							EMIT3("set",q,"36",p);
							EMIT2( (modifier&1)?"sumv":"subv",q,(m=='F')?"F1":"N1");
							EMIT2("sindv",p,q);
							if (modifier>2) EMIT2( (modifier==3)?"subv":"sumv",q,(m=='F')?"F1":"N1");
							freeTemp(c,js);
							strcpy(c->val,j);
						} else {
							if ((s[*ii]=='+')&&(s[*ii+1]=='+')) {*ii+=2; modifier=3; goto mooo;}
							if ((s[*ii]=='-')&&(s[*ii+1]=='-')) {*ii+=2; modifier=4; goto mooo;}
						}
						return M_NOERR;
					}
					k=atoi(c->val+1);
					if (k<0) return error3(157,k);
					if (isParameterMatrix(js)) {
						js=getTemp(c);
						if (js==NULL) return M_FAIL;
						p=getName(c,js);
						if (k==0) {
							if (modifier) {
								EMIT2("setv",p,name);
								EMIT1("indv",p);
							} else EMIT3("set",p,"36",name);
						} else {
							EMIT2("setv",p,c->val);
moo:						EMIT2("sumv",p,name);
							EMIT1("indv",p);
						}
						strcpy(c->val,js);
						goto mo;
					}
					if (m!='B') {
						f+=(name[0]!='T')?k:-k;
						itoa(f,name+1+(m=='F'),10);
					} else {
						if (k>=15) return error3(158,k);
						itoa(f*100+k,name+2,10);
					}
					if (m!='I') name[1]=m;
					js=&name[0];
				}
				c->cons=C_VARIABLE;
				c->type=C_INTEGER;
				if (isBool(js)) c->type=C_BOOL;
				if (isFloat(js)) c->type=C_FLOAT;
				if (isArray(js)) {
					if (modifier) return error(159);
					m=atoi(js+2);
					if (s[*ii]!='[') return error(160);
					*ii+=1;
					failure=expr(s,ii,ls,c);
					if (failure!=M_NOERR) return failure;
					if (s[*ii]!=']') return error(153);
					*ii+=1;
					if (!isInteger(c->cons)) {
						if (!iscInteger(c)) return error(154);
						if (isTemp(c->val)) {
							freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));
						}
						if (c->task!=0) return error(161);
						EMIT3("setv","L1","2",js+2);
						EMIT2("setv","L2",c->val);
						EMIT1("calls","7");
						arrayg[m]=1;
						f=(arrayt[m]==C_INTEGER)?1:(arrayt[m]==C_FLOAT)?3:2;
						goto arr;
					}
					k=atoi(c->val+1);
					if ((k<0)||(k>=arrayn[m])) return error3(162,arrayn[m]);
					c->cons=arrayt[m];
					c->type=arrayt[m];
					getArray(m,k,c->val);
					return M_NOERR;
				}
				if ((s[*ii]=='+')&&(s[*ii+1]=='+')) {*ii+=2; modifier=3;}
				else if ((s[*ii]=='-')&&(s[*ii+1]=='-')) {*ii+=2; modifier=4;}
				if (modifier&&(!isWritable(js)||isBool(js))) return error(163);
				if (modifier==1) EMIT2("sumv",js,isFloat(js)?"F1":"N1");
				if (modifier==2) EMIT2(
					"sumv",js,isFloat(js)?"F-1":"N-1");
				if (modifier==3) {strcpy(c->val,js);strcat(c->val,"+");}
				else if (modifier==4) {
					strcpy(c->val,js);strcat(c->val,"-");}
				else strcpy(c->val,js);
			} else return error(164);
		}
		return M_NOERR;
    } else if (s[i]!='(') return error(165);
	if (modifier) return error(166);
	*ii=i+1;
	failure = expr (s,ii,ls,c);
	if ( failure!=M_NOERR ) return failure;
	if (W(s,ii,ls,&c->lineno)) return M_FAIL;
	if (s[*ii]!=')') return error(167);
	*ii+=1;
	if (W(s,ii,ls,&c->lineno)) return M_FAIL;
	if (s[*ii]=='?') {int j; char temp4[4],*ps,*pps,temp5[8];
		*ii+=1;
		if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
		else if (iscBool(c)&&!iscBoolParameter(c)) {
			setBooleanBase(c->val,temp4,&m);
			ps=getTemp(c);
			if (ps==NULL) return M_FAIL;
			strcpy(c->val,pps=getName(c,ps));
			EMIT2("setv",pps,temp4);
			temp5[0]='N'; itoa((1<<m),temp5+1,10);
			EMIT2("andv",pps,temp5);
			freeTemp(c,ps);
		}
		j=getLabel()*10000+getLabel();
		EMIT4("chkl",c->val,"3",iscFloat(c)?"F0":"N0",makeLabel(j/10000));
		failure = expr (s,ii,ls,c);
		if ( failure!=M_NOERR ) return failure;
		if (W(s,ii,ls,&c->lineno)) return M_FAIL;
		if (s[*ii]!=':') return error(168);
		*ii+=1;
		i=c->type;
		if (isTemp(c->val)) {ps=c->val; strcpy(temp5,getName(c,c->val));}
		else {
			ps=getTemp(c);
			if (ps==NULL) return M_FAIL;
			strcpy(temp5,getName(c,ps));
			if (iscBool(c)&&!iscBoolParameter(c)) {
				setBooleanBase(c->val,temp4,&m);
				EMIT2("setv",temp5,temp4);
				temp4[0]='N'; itoa((1<<m),temp4+1,10);
				EMIT2("andv",temp5,temp4);
				EMIT2("divv",temp5,temp4);
			} else EMIT2("setv",temp5,c->val);
		}
		EMIT1("jmpl",makeLabel(j%10000));
		EMIT(strcat(makeLabel(j/10000),":"));
		failure = expr (s,ii,ls,c);
		if ( failure!=M_NOERR ) return failure;
		if (i!=c->type) return error(169);
		if (W(s,ii,ls,&c->lineno)) return M_FAIL;
		if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
		if (iscBool(c)&&!iscBoolParameter(c)) {
				setBooleanBase(c->val,temp4,&m);
				EMIT2("setv",temp5,temp4);
				temp4[0]='N'; itoa((1<<m),temp4+1,10);
				EMIT2("andv",temp5,temp4);
				EMIT2("divv",temp5,temp4);
		} else EMIT2("setv",temp5,c->val);
		EMIT(strcat(makeLabel(j%10000),":"));
		c->cons=C_RESULT;
		strcpy(c->val,ps);
	}
	return M_NOERR;
}
static int isCommutative(int op) {
	return ((op==ADD)||(op==MULTIPLY)||(op==AND)||(op==OR)||(op==XOR)||(op==EQL)||(op==NEQL));
}
#define isResult(x) ((x)==C_RESULT)

static void turnOff(char *val) {
	int i=strlen(val)-1;
	if ((val[i]=='-')||(val[i]=='+')) val[i]=0;
}

static int isPlusMinus(char *val) {
	int i=strlen(val)-1;
	return ((val[i]=='-')||(val[i]=='+'));
}

static int divide(char *pps, Context *c) {
	int j;
			if (iscFloat(c)) {float ii;
				if (isNumber(c->cons)) {
					ii=(float)atof(c->val+1);
					if (ii==0) return error(170);
					j=(int)(ii);
					if (float(j)!=ii) goto zd;
					ftoa(ii/10.0f,c->val+1,10);
					EMIT2("divv",pps,c->val);
				} else {
zd:					EMIT2("mulv",pps,"F1");
					EMIT2("divv",pps,c->val);
				}
			} else {
				EMIT2("divv",pps,c->val);
				if (isNumber(c->cons)) {float ii;
					ii=(float)atof(c->val+1);
					if (ii==0) return error(170);
				}
			}
			return M_NOERR;
}
//C op C
//C op V
//C op R
//V op C
//V op V
//V op R
//R op C
//R op V
//R op R

static int op2(int j,Context *c,int cons,int ct,char *val) {//result in c
	int i,k; char *ps,*pps,*qs,*qqs,temp5[8]; char temp4[8];
	if ((cons==c->cons)&&(isNumber(c->cons)||isBool(c->cons))) {float ii,kk;
		if (isBool(cons)) {
			i=val[1]=='1';  k=c->val[1]=='1';
			if (j==AND) i=i&&k;
			else if (j==ANDAND) i=i&&k;
			else if (j==OROR) i=i||k;
			else if (j==OR) i=i||k;
			else if (j==XOR) i=i^k;
			else if (j==EQL) i=i==k;
			else if (j==NEQL) i=i!=k;
zbb:		c->cons=C_BOOL; c->type=C_BOOL; strcpy(c->val,"N0");
			if (i==0) c->val[1]='0'; else c->val[1]='1';
			return M_NOERR;
		}
		ii=(float)atof(val+1); kk=(float)atof(c->val+1);
		if (j==EQL) {i= ii==kk; goto zbb;}
		if (j==NEQL) {i= ii!=kk; goto zbb;}
		if (j==GEQ) {i= ii>=kk; goto zbb;}
		if (j==GTR) {i= ii>kk; goto zbb;}
		if (j==LEQ) {i= ii<=kk; goto zbb;}
		if (j==LSS) {i= ii<kk; goto zbb;}
		if (j==ADD) ii+=kk;
		if (j==SUBTRACT) ii-=kk;
		if (j==MULTIPLY) ii*=kk;
		if (j==INTDIV) {
			if (kk==0) return error(171);
			if (isInteger(cons)) ii=(float)(((int)ii)/(int)kk);
			else ii/=kk;
		}
		if (j==POWER) 
			ii=(float)pow(ii,kk);
		if (j==SHIFTL) {
			if (!isInteger(cons)) return error(172);
			ii=(float)(((int)ii)<<(int)kk);
		}
		if (j==SHIFTR) {
			if (!isInteger(cons)) return error(173);
			ii=(float)(0xffff&((int)ii)>>(int)kk);
		}
		if (j==MOD) {
			if (!isInteger(cons)) return error(174);
			if (kk==0) return error(175);
			ii=(float)(((int)ii)%(int)kk);
		}
		if (j==XOR) {
			if (!isInteger(cons)) return error(176);
			ii=(float)(((int)ii)^(int)kk);
		}
		if (j==AND) {
			if (!isInteger(cons)) return error(177);
			ii=(float)(((int)ii)&(int)kk);
		}
		if (j==ANDAND) {i=ii&&kk; goto zbb;}
		if (j==OROR) {i=ii||kk; goto zbb;}
		if (j==OR) {
			if (!isInteger(cons)) return error(178);
			ii=(float)(((int)ii)|(int)kk);
		}
		if (isFloat(cons)) ftoa(ii,c->val+1,10);
		else {
			if ((ii<-32768)||(ii>32767)) return error3(179,(int)ii);
			itoa((int)ii,c->val+1,10);
		}
		return M_NOERR;
	}
	if (ct!=c->type) return M_FAIL;
	if (isCommutative(j)) {
		if ((isResult(c->cons)&&!isResult(cons))||
			isNumber(cons)||isBool(cons)) {char x[8];
			i=cons;  cons=c->cons;  c->cons=i;
			i=ct;  ct=c->type;  c->type=i;
			strcpy(x,val);  strcpy(val,c->val);  strcpy(c->val,x);
		}
	}
	if (isTemp(val)) {
		ps=val;
		pps=getName(c,val);
	} else {
		ps=getTemp(c);
		if (ps==NULL) return M_FAIL;
		pps=getName(c,ps);
	}
	/*if ((isBoolParameter(val)||isResult(cons))&&(iscBoolParameter(c)||isResult(c->cons))) {
	} else*/ if (iscBool(c)) {int m; char temp4[4],*qs;
		//if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
		//if (isResult(cons)&&(iscBoolParameter(c)||isResult(c->cons))) {
		//	strcpy(temp5,c->val);
		//	goto zb;
		//}
		if (!isResult(cons)) {
			setBooleanBase(val,temp4,&m);
			if (isBoolParameter(val)) {
				EMIT2("setv",pps,val);  m=0;
			} else {
				EMIT2("setv",pps,temp4);
				temp5[0]='N'; itoa((1<<m),temp5+1,10);
				EMIT2("andv",pps,temp5);
				if (m!=0) EMIT2("divv",pps,temp5);
				cons=C_RESULT;  ct=C_BOOL; strcpy(val,ps);
			}
		}
		if (!isResult(c->cons)&&!iscBoolParameter(c)) {
			qs=getTemp(c);
			if (qs==NULL) return M_FAIL;
			qqs=getName(c,qs);
			if (iscConstant(c)) {
				EMIT2("setv",qqs,c->val);
			} else {
				setBooleanBase(c->val,temp4,&m);
				EMIT2("setv",qqs,temp4);
				temp5[0]='N'; itoa((1<<m),temp5+1,10);
				EMIT2("andv",qqs,temp5);
				if (m!=0) EMIT2("divv",qqs,temp5);
			}
			c->cons=C_RESULT;  c->type=C_BOOL; strcpy(c->val,qs);
		}
	}
	if ((j!=EQL)&&(j!=NEQL)&&(j!=GTR)&&(j!=GEQ)&&(j!=LSS)&&(j!=LEQ)&&(j!=XOR)) {
		if ((!isTemp(val))&&(j!=MOD)) {
			EMIT2("setv",pps,val); turnOff(val);
		}
		if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
	} //move to rel op section else if (isTemp(val)&&(ct!=C_BOOL)) {
	  //	ps=getTemp(c);              //overwrites previous temp allocation??
	  //	strcpy(temp4,getName(c,ps)); pps=temp4;}
	if (j==ADD) EMIT2("sumv",pps,c->val);
	else if (j==SUBTRACT) EMIT2("subv",pps,c->val);
	else if (j==AND) {
		if (!iscInteger(c)&&!iscBool(c)) return error(180);
		EMIT2("andv",pps,c->val);
	}else if (j==OR) {
		if (!iscInteger(c)&&!iscBool(c)) return error(181);
		EMIT2("orv",pps,c->val);
	}else if (j==POWER) {float ii;
		if (!isNumber(c->cons)||isTemp(val)) return error(182);
		ii=(float)atof(c->val+1);
		if (((float)(int)ii)!=ii) return error(183);
		if (ii==0.0f) {
			UNEMIT();
			freeTemp(c,ps);
			c->cons=c->type;
			if (iscFloat(c)) strcpy(c->val,"F1"); else strcpy(c->val,"N1");
			return M_NOERR;
		}
		if (ii==1.0f) {
			UNEMIT();
			freeTemp(c,ps);
			return M_NOERR;
		}
		if (ii==-1.0f) {
			UNEMIT();
			if (iscFloat(c)) {
				EMIT2("setv",pps,"F1");
				EMIT2("divv",pps,val);
			} else {
				strcpy(c->val,"N0");
			}
			return M_NOERR;
		} else if ((ii==2.0f)||(ii==4.0f)) {
			EMIT2("mulv",pps,pps);
			if (iscFloat(c)) EMIT2("divv",pps,"F1");
			if (ii==4.0f) {
				EMIT2("mulv",pps,pps);
				if (iscFloat(c)) EMIT2("divv",pps,"F1");
			}
		} else return error(184);
	} else if (j==MULTIPLY) {
			if (iscFloat(c)) {float ii;
				if (isNumber(c->cons)) {
					ii=(float)atof(c->val+1);
					j=int(ii);
					if (float(j)!=ii) goto zf;  /*TODO: may need subtract for check*/
					ftoa(ii/10.0f,c->val+1,10);
					EMIT2("mulv",pps,c->val);
				} else {
zf:					EMIT2("mulv",pps,c->val);
					EMIT2("divv",pps,"F1");
				}
			} else EMIT2("mulv",pps,c->val);
	} else if (j==XOR) { char *qs2,*qqs2;//(a|b)&~(a&b)
		if (!iscInteger(c)&&!iscBool(c)) return error(185);
		qs=getTemp(c);
		if (qs==NULL) return M_FAIL;
		qqs=getName(c,qs);
		if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
		qs2=getTemp(c);
		if (qs2==NULL) return M_FAIL;
		qqs2=getName(c,qs2);
		turnOff(pps);  turnOff(c->val);
		EMIT2("setv",qqs,c->val);
		EMIT2("orv",qqs,pps);
		EMIT2("andv",pps,c->val);
		freeTemp(c,qs); freeTemp(c,qs2);
		if (iscBool(c)) EMIT2("setv",qqs2,"N1"); else EMIT2("setv",qqs2,"N-1");
		EMIT2("subv",qqs2,pps);
		EMIT2("andv",pps,qqs2);
	} else if (j==INTDIV) {
		if (i=divide(pps,c)!=M_NOERR) return i;
	} else if (j==MOD) {
			strcpy(temp5,val); turnOff(temp5);
			EMIT2("setv",pps,temp5);
			if (!iscInteger(c)) return error(186);
			if (isNumber(c->cons)) {float ii;
				ii=(float)atof(c->val+1);
				if (ii==0) return error(187);
			}
			if (isNumber(c->cons)&&((i=pow2(c->val))!=0)) {
				itoa((1<<i)-1,c->val+1,10);
				EMIT2("andv",pps,c->val);
			} else {
				if (isSpecial(c->val)) return error(188);
				if (isSpecial(val)) {
					qs=getTemp(c);
					if (qs==NULL) return M_FAIL;
					qqs=getName(c,qs);
					EMIT2("setv",qqs,pps);
				} else qs=NULL;
				strcpy(temp4,c->val); turnOff(temp4);
				EMIT2("divv",pps,temp4);
				EMIT2("mulv",pps,c->val);
				EMIT2("subv",pps,isSpecial(val)?qqs:val);
				EMIT2("mulv",pps,"N-1");
				if (isSpecial(val)) freeTemp(c,qs);
			}
	} else if ((j!=SHIFTL)&&(j!=SHIFTR)) {char *label=NULL,*label2=NULL; //jump false 0> 1< 2= 3!=
				if (isPlusMinus(val)) {
					label=getTemp(c);
					if (label==NULL) return M_FAIL;
					strcpy(temp4,getName(c,label));
					EMIT2("setv",temp4,val); strcpy(val,temp4);
				}
				if (isPlusMinus(c->val)) {
					label2=getTemp(c);
					if (label2==NULL) return M_FAIL;
					strcpy(temp4,getName(c,label2));
					EMIT2("setv",temp4,c->val); strcpy(c->val,temp4);
				}
				if (label!=NULL) freeTemp(c,label);
				if (label2!=NULL) freeTemp(c,label2);
				k=getLabel();
				label=makeLabel(k);
				if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
				if (isTemp(val)) {
					ps=getTemp(c);
					if (ps==NULL) return M_FAIL;
					pps=getName(c,ps);
					freeTemp(c,val); val=getName(c,val);
				}
				EMIT2("setv",pps,"N0");
				/* 0 >  1 <  2 ==  3 !=  jump on condition false*/
				if (j==EQL) {EMIT4("chkl",c->val,"2",val,label); goto rr;}
				if (j==NEQL) {
					EMIT4("chkl",c->val,"3",val,label); goto rr;}
				if (j==LSS) {
					if (isNumber(c->cons)) 
zg:						EMIT4("chkl",c->val,"0",val,label);
					else 
zgg:					EMIT4("chkl",val,"1",c->val,label);					
				} else if (j==GTR) {
					if (isNumber(c->cons)) 
zl:						EMIT4("chkl",c->val,"1",val,label);
					else 
zll:					EMIT4("chkl",val,"0",c->val,label);
				} else if (j==LEQ) { /*x<=7 == x<8 == !(8>x)*/
					if (isNumber(c->cons)) {inc(c,1); goto zg;}
					if (isNumber(cons)) {incs(val,-1); goto zgg;} /*7<=x == 6<x*/
					k=getLabel();
					label2=makeLabel(k);
					EMIT4("chkl",val,"3",c->val,label2);
					EMIT4("chkl",val,"1",c->val,label);
					EMIT(strcat(label2,":"));
				}
				else if (j==GEQ) {
					if (isNumber(c->cons)) {inc(c,-1); goto zl;}
					if (isNumber(cons)) {incs(val,1); goto zll;}
					k=getLabel();
					label2=makeLabel(k);
					EMIT4("chkl",val,"3",c->val,label2);
					EMIT4("chkl",val,"0",c->val,label);
					EMIT(strcat(label2,":"));
				}
rr:				EMIT2("setv",pps,"N1");
				EMIT(strcat(label,":"));
				c->type=C_BOOL; c->cons=C_RESULT;
				strcpy(c->val,ps);
				return M_NOERR;
	} else {
				if (!isInteger(c->cons)) return error(189);
				i=atoi(c->val+1);
				itoa(1<<i,c->val+1,10);
				if (j==SHIFTL) EMIT2("mulv",pps,c->val);
				else EMIT2("divv",pps,c->val);
	}
	c->cons=C_RESULT;
	strcpy(c->val,ps); 
	return M_NOERR;
}
static int term(char *s, int *ii, int ls, Context *c)
{   int failure,b,i,ct; char val[VAL_LIMIT];
    failure = factor (s,ii,ls,c);
    for (;failure==M_NOERR;)
	{
	  if (W(s,ii,ls,&c->lineno)) return M_FAIL;
	  switch (s[*ii]) {
      case '=' :
			b=EQL;
		    *ii+=1;
		    if (s[*ii]!='=' ) return error(190);
zz:			*ii+=1;
			i=c->cons;  ct=c->type;
			strcpy(val,c->val);
			c->cons=C_VOID;
			failure = term (s,ii,ls,c);
			if (failure!=M_NOERR) return failure;
			if (failure=op2(b,c,i,ct,val)) return failure;
			continue;
      case '!' : b=NEQL; *ii+=1; if (s[*ii]!='=') return error(191); goto zz;
      case '>' :
			if (s[*ii+1]=='=' ) {b=GEQ; *ii+=1; goto zz;}
			else if (s[*ii+1]=='>') {
				b=SHIFTR; *ii+=1; goto zz;}
			b=GTR; goto zz;
      case '<' :
			if (s[*ii+1]=='=' ) {b=LEQ; *ii+=1; goto zz;}
			else if (s[*ii+1]=='<') {
				b=SHIFTL; *ii+=1; goto zz;}
			b=LSS; goto zz;
      case '*':
	    b=MULTIPLY;
		if (s[(*ii)+1]=='*') {
			b=POWER; *ii+=1;}
z:      *ii+=1;
		i=c->cons;  ct=c->type;
		strcpy(val,c->val);
		c->cons=C_VOID;
		failure = factor (s,ii,ls,c);
		if (failure!=M_NOERR) return failure;
		if (failure=op2(b,c,i,ct,val)) return failure;
		continue;
	  case '/':
		  b=INTDIV; goto z;
	  case '%':  b=MOD; goto z;
	  case '^':  b=XOR; goto z;
	  default:
		  return failure;
	  }
    }
    return failure;
}

static int expr(char *s, int *ii, int ls, Context *c)
{   char val[VAL_LIMIT]; char *ps,*pps;
	int failure,i,j,k;
	c->cons=C_VOID;
    if (W(s,ii,ls,&c->lineno)) return M_FAIL;
	if (( s[*ii] == '+' )&&(s[*ii+1]!='+')) {
		j=UNARYPLUS;
		*ii+=1; 
		failure = term (s,ii,ls,c);
		if (failure!=M_NOERR) return failure;
		if (!iscInteger(c)&&!iscFloat(c)) return error(192);
    } else if (( s[*ii] == '-' )&&(s[*ii+1]!='-')) {
		j=INVERT;
zz:     *ii+=1;
		failure = factor(s,ii,ls,c);
		if (failure!=M_NOERR) return failure;
		if (isNumber(c->cons)) {float ii;
			ii=(float)atof(c->val+1);
			if (j==INVERT) ii=-ii;
			if (j==NOT) {ii=ii==0; c->type=C_BOOL;  c->cons=C_BOOL;}
			if (j==FLIP) {
				if (isFloat(c->cons)) return error(193);
				ii=(float)~(int)ii;
			}
			ftoa(ii,c->val+1,10);
		} else if (isBool(c->cons)) {
			if (j!=NOT) return error(194);
			c->val[1]=(c->val[1]=='0')?'1':'0';
		} else {
			ps=getTemp(c);
			if (ps==NULL) return M_FAIL;
			pps=getName(c,ps);
			if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
			if (j==INVERT) {
				if (!iscFloat(c)&&!iscInteger(c)) return error(195);
				EMIT2("setv",pps,iscFloat(c)?"F0":"N0");
				EMIT2("subv",pps,c->val);
			}
			if (j==FLIP) {
				if (!iscInteger(c)) return error(196);
				EMIT2("setv",pps,iscFloat(c)?"F-1":"N-1");
				EMIT2("subv",pps,c->val);
			}
			if (j==NOT) {
				if (iscInteger(c)) {char *label;
					j=getLabel();
					label=makeLabel(j);
					EMIT2("setv",pps,"N0");
					EMIT4("chkl",c->val,"2","N0",label);
					EMIT2("setv",pps,"N1");
					EMIT(strcat(label,":"));
					c->type=C_BOOL;
				} else if (!iscBool(c)) return error(197);
				else if (iscBoolParameter(c)||isResult(c->cons)) {
					EMIT2("setv",pps,"N1");
					EMIT2("subv",pps,c->val);
				} else {char temp2[8],temp[4];
					setBooleanBase(c->val,temp,&i);
					temp2[0]='N';
					itoa((1<<i),temp2+1,10);
					EMIT2("setv",pps,"N-1");
					EMIT2("subv",pps,temp);
					EMIT2("andv",pps,temp2);
					if (i!=0) EMIT2("divv",pps,temp2);
				}
			} 
			c->cons=C_RESULT;
			strcpy(c->val,ps);
		}
	} else if ( s[*ii] == '!' ) {j=NOT; goto zz;
	} else if ( s[*ii] == '~' ) {j=FLIP; goto zz;
    } else {
		//if not + or - go to term.
        failure = term (s,ii,ls,c);
    }
    for (;;)
	{
      if ( failure!=M_NOERR ) break;
	  if (W(s,ii,ls,&c->lineno)) return M_FAIL;
      switch ( s[*ii])
	  {int ct;
		case '+' :
			j=ADD;
b:			*ii+=1;
			i=c->cons;  ct=c->type;
			strcpy(val,c->val);
			c->cons=C_VOID;
			failure = term (s,ii,ls,c);
			if (failure!=M_NOERR) return failure;
			if (failure=op2(j,c,i,ct,val)) return failure;
			continue;
		case '-' : j=SUBTRACT; goto b;
        case '&' :
			j=AND;
			if (s[*ii+1]!='&') goto b;
			k=ANDAND;
zb:			*ii+=2;  j=getLabel();
			ps=getTemp(c);
			if (ps==NULL) return M_FAIL;
			strcpy(val,pps=getName(c,ps));
			if (k==ANDAND) EMIT2("setv",val,"N0"); else EMIT2("setv",val,"N1");
			if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
			else if (iscBool(c)&&!iscBoolParameter(c)&&!iscConstant(c)) {char temp4[4],*ps,*pps,temp5[8]; int m;
				setBooleanBase(c->val,temp4,&m);
				ps=getTemp(c);
				if (ps==NULL) return M_FAIL;
				strcpy(c->val,pps=getName(c,ps));
				EMIT2("setv",pps,temp4);
				temp5[0]='N'; itoa((1<<m),temp5+1,10);
				EMIT2("andv",pps,temp5);
				freeTemp(c,ps);
			}
			EMIT4("chkl",c->val,(k==ANDAND)?"3":"2",iscFloat(c)?"F0":"N0",makeLabel(j));
			if (W(s,ii,ls,&c->lineno)) return M_FAIL;
			c->cons=C_VOID;
			failure = term (s,ii,ls,c);
			if (iscBool(c)&&isResult(c->cons)) {
				if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
				EMIT2("setv",val,c->val);
			} else if (iscBool(c)&&!iscBoolParameter(c)&&!iscConstant(c)) {char temp4[4],*ps,*pps,temp5[8]; int m;
				setBooleanBase(c->val,temp4,&m);
				ps=getTemp(c);
				if (ps==NULL) return M_FAIL;
				strcpy(c->val,pps=getName(c,ps));
				EMIT2("setv",pps,temp4);
				temp5[0]='N'; itoa((1<<m),temp5+1,10);
				EMIT2("andv",pps,temp5);
				freeTemp(c,ps);
				EMIT4("chkl",c->val,(k==ANDAND)?"3":"2","N0",makeLabel(j));
				if (k==ANDAND) EMIT2("setv",val,"N1"); else EMIT2("setv",val,"N0");
			} else {
				EMIT4("chkl",c->val,(k==ANDAND)?"3":"2",iscFloat(c)?"F0":"N0",makeLabel(j));
				if (k==ANDAND) EMIT2("setv",val,"N1"); else EMIT2("setv",val,"N0");
			}
			if (failure!=M_NOERR) return failure;
			EMIT(strcat(makeLabel(j),":"));
			c->cons=C_RESULT; c->type=C_BOOL; strcpy(c->val,ps);
			continue;
        case '|' :
			j=OR;
			if (s[*ii+1]!='|') goto b;
			k=OROR;
			goto zb;
      default:
		  return failure;
      }
    }
    return failure;
}
static char scass[]="+=-=*=/=%=&=|=^=";
static int cass(char *s) {
	if ((s[0]=='+')&&(s[1]=='+')) return 1;
	if ((s[0]=='-')&&(s[1]=='-')) return 2;
	for (int j=0; j<8; j++)
		if ((s[0]==scass[j+j])&&(s[1]==scass[j+j+1])) return j+3;
	return 0;
}
static int condtest(char *s, int *ii, int ls, Context *c, int j) {
	int m;
		if (expr(s,ii,ls,c)!=M_NOERR) return 1;
		if (isTemp(c->val)) {
			freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));
		} else if (iscConstant(c)) {
			if (!iscConstantTrue(c)) {
				EMIT1("jmpl",makeLabel(j/10000));
			}
			goto zif1;
		} else if (iscBool(c)&&!iscBoolParameter(c)) {char temp4[4],*ps,*pps,temp5[8];
			setBooleanBase(c->val,temp4,&m);
			ps=getTemp(c);
			if (ps==NULL) return 1;
			strcpy(c->val,pps=getName(c,ps));
			EMIT2("setv",pps,temp4);
			temp5[0]='N'; itoa((1<<m),temp5+1,10);
			EMIT2("andv",pps,temp5);
			freeTemp(c,ps);
		}
		EMIT4("chkl",c->val,"3",iscFloat(c)?"F0":"N0",makeLabel(j/10000));
zif1:	if (W(s,ii,ls,&c->lineno)) return 1;
		return 0;
}
static int state(char *s, int *ii, int ls, Context *c) {
  int failure=M_NOERR,j,i,m,k,b,kludge=false;  char *js;
  char * iffortype[10];
  int taskin=0, subin=0, gotmain=0, forstate=0;
  int ifforaddr[10], iffor=0, isReturn, isUnion=0, isConst=0;
  char name[36];
  char ss[128], *tps, *ts; int ti,tls,tj,tk; /*not useable for any other purpose*/
  for (i=*ii;failure==M_NOERR;*ii=i) {
	if (kludge) {EMIT("rets"); kludge=false;}
	if (W(s,&i,ls,&c->lineno)) {
stop:		*ii=i; return M_FAIL;
ok:			*ii=i;
			if (iffor>0) return error(2);
			if (isUnion) return error(3);
			if (taskin) return error(4);
			if (subin) return error(5);
			if (gotmain==0) return error(6);
			return M_NOERR;
	}
	if (i>=ls)
	{
			goto ok;
	}
	if (forstate==1) {
		if (s[i]!=';') {error(205); goto stop;}
		i++;
		if ((s[i]==';')&&(s[i+1]==')')) {
			js=NULL; i+=2; forstate=0; goto zz2;
		}
		tj=getLabel()*10000+getLabel();
		tk=getLabel();
		ifforaddr[iffor]=tj;
		iffortype[iffor]=NULL; iffor++;
		js=strchr(s+i,';');
		if (js==NULL) {error(206); goto stop;}
		tps=strchr(js+1,'{');
		if (tps==NULL) {error(38); goto stop;}
		strncpy(ss,js+1,tps-js+1);
		ss[tps-js+1]=0;
		for (j=0;ss[j]==' ';j++);
		if (ss[j]!=')') EMIT1("jmpl",makeLabel(tk));
		EMIT(strcat(makeLabel(tj%10000),":"));
		if (ss[j]==')') goto zz223;
		ts=s; ti=i; tls=ls;
		s=ss+j; i=0; ls=tps-js+1-j;
		if (W(s,&i,ls,&c->lineno)) {i=ti; goto stop;}
		if (!isalpha(s[i])) {error(207); i=ti; goto stop;}
		forstate=2; goto zz22;
	} else if (forstate==2) {
		EMIT(strcat(makeLabel(tk),":"));
		s=ts; i=ti; ls=tls;
zz223:	if (W(s,&i,ls,&c->lineno)) goto stop;
		if (s[i]==';') goto zz222;
		if (condtest(s,&i,ls,c,tj)) goto stop;
		if (s[i++]!=';') {error(206); goto stop;}
zz222:	i=tps-s;
		if (s[i++]!='{') {error(38); goto stop;}
		forstate=0;
		continue;
	}
	//if ; then skip char
    if (s[i]==';')
	{
		i++;
		continue;
	}
	if (s[i]=='}') {
		if (forstate) {error(205); goto stop;}
		i++;
		if (isUnion) {
			isUnion=0; unioning=-1; isConst=0;
			continue;
		}
		j=7; goto zz;
	}
zz22:	if (((s[i]>='a')&&(s[i]<='z'))||((s[i]>='A')&&(s[i]<='Z')))
	{char temp[8];
		j=s[i++];
		m=strspn(s+i,"abcdefg_hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		strncpy(name,s+i-1,m+1); name[m+1]=0;
		i+=m;
		if (W(s,&i,ls,&c->lineno)) goto stop;
		if (m>0) {
			if (m>35) {error2(8,name); goto stop;}
			if ((j=='i')&&(strcmp(name,"int")==0))  {b=0; j=9; goto zz;}
			if ((j=='s')&&(strcmp(name,"short")==0))  {b=0; j=9; goto zz;}
			if ((j=='c')&&(strcmp(name,"char")==0))  {b=0; j=17; goto zz;}
			if ((j=='f')&&(strcmp(name,"float")==0)) {b=0; j=14; goto zz;}
			if ((j=='b')&&(strcmp(name,"bool")==0)) {b=0; j=13; goto zz;}
			if (isUnion||isConst) goto stop;
			if ((j=='c')&&(strcmp(name,"const")==0))  {isConst=true; continue;}
			if ((j=='t')&&(strcmp(name,"task")==0))  {j=10; goto zz;}
			if ((j=='v')&&(strcmp(name,"void")==0)) {j=12; goto zz;}
			if ((j=='u')&&(strcmp(name,"union")==0)) {
				isUnion=1; unioning=0;
				if (W(s,&i,ls,&c->lineno)) goto stop;
				if (s[i++]!='{') {error(9); goto stop;}
				continue;
			}
			if ((c->task<0)||((taskin==0)&&(subin==0))) {error(10); goto stop;}
			if ((j=='r')&&(strcmp(name,"return")==0)) {j=8; goto zz;}
			if ((j=='f')&&(strcmp(name,"for")==0))	 {j=2; goto zz;}
			if ((j=='b')&&(strcmp(name,"break")==0))  {j=3; goto zz;}
			if ((j=='c')&&(strcmp(name,"continue")==0)) {j=4; goto zz;}
			if ((j=='i')&&(name[1]=='f')&&(name[2]==0))   {j=5; goto zz;}
			if ((j=='a')&&(strcmp(name,"asm")==0)) {j=6; goto zz;}
			if ((j=='w')&&(strcmp(name,"while")==0)) {j=11; goto zz;}
			if ((j=='s')&&(strcmp(name,"start")==0)) {j=15; goto zz;}
			if ((j=='s')&&(strcmp(name,"stop")==0)) {j=16; goto zz;}
			if ((j=='s')&&(strcmp(name,"switch")==0)) {error(41); goto stop;}
			if ((j=='d')&&(strcmp(name,"do")==0)) {error(42); goto stop;}
			if ((j=='r')&&(strcmp(name,"repeat")==0)) {error(43); goto stop;}
			if ((j=='u')&&(strcmp(name,"until")==0)) {error(44); goto stop;}
		}
		m=c->nNames;
		if ((js=findName(c,name))==NULL) {error2(11,name); goto stop;}
		if (isProcedure(js)&&(forstate==0)) {
			if (doProcedure(c,s,i,ls,b,js)!=M_NOERR) goto stop;
			continue;
		}
		if ((c->task<0)||((taskin==0)&&(subin==0))) {error(10); goto stop;}
		if (!isWritable(js)) {error2(12,name); goto stop;}
		if (isMatrix(js)) {int n,z; char *p,*q,*r;
		if (s[i++]!='[') {error(13); goto stop;}
			j=atoi(js+3);  b=js[2];  z=js[1];
			name[0]=js[0]; name[1]=0; strcat(name,js+3);
			if (expr(s, &i, ls, c)!=M_NOERR) goto stop;
			if (s[i++]!=']') {error(14); goto stop;}
			if (c->cons!=C_INTEGER) {
				if (!iscInteger(c)) {error(15); goto stop;}
				if (W(s,&i,ls,&c->lineno)) goto stop;
				k=cass(s+i);
				if ((k!=0)&&(b=='B')) {error(16); goto stop;}
				if (isTemp(c->val)) {
					js=c->val;
					p=getName(c,c->val);
				} else {
					js=getTemp(c);
					if (js==NULL) goto stop;
					p=getName(c,js);
					EMIT2("setv",p,c->val);
				}
				if (z=='Y') EMIT2("sumv",p,name);
				else if (b!='B') EMIT2("addav",p,name);
mo:				if (k!=0) i+=2;
				if (W(s,&i,ls,&c->lineno)) goto stop;
				if ((k!=0)&&(k<3)) {
					q=getTemp(c);
					if (q==NULL) goto stop;
					r=getName(c,q);
					EMIT3("set",r,"36",p);
					EMIT2((k==1)?"sumv":"subv",r,b?"F1":"N1");
					EMIT2("sindv",p,r);
					freeTemp(c,q);  freeTemp(c,js);
					continue;
				}
				if ((k==0)&&(s[i++]!='=')) {error(17); goto stop;}
				failure=expr(s,&i,ls,c);
				if (failure!=M_NOERR) goto stop;
				if ((b=='F')&&!iscFloat(c)) {error(18); goto stop;}
				if ((b=='I')&&!iscInteger(c)) {error(18); goto stop;}
				if ((b=='B')&&!iscBool(c)) {error(18); goto stop;}
				q=c->val;
				if (isTemp(q)) q=getName(c,c->val);
				if (k!=0) {char *s;
					r=getTemp(c);
					if (r==NULL) goto stop;
					s=getName(c,r);
					EMIT3("set",s,"36",p);
					if (k==3) EMIT2("sumv",s,q);
					else if (k==4) EMIT2("subv",s,q);
					else if (k==5) {
						EMIT2("mulv",s,q);
						if (b=='F') EMIT2("divv",s,"F1");
					} else if (k==6) {
						if (b=='F') EMIT2("mulv",s,"F1");
						EMIT2("divv",s,q);
					} else if (k==7) {char *t,*u;
						if (b!='I') {error(19); goto stop;}
						t=getTemp(c);
						if (t==NULL) goto stop;
						u=getName(c,t);
						EMIT2("setv",u,s);
						EMIT2("divv",u,q);
						EMIT2("mulv",u,q);
						EMIT2("subv",s,u);
						freeTemp(c,t);
					} else {
						if (b=='F') {error(20); goto stop;}
						if (k==8) EMIT2("andv",s,q);
						else if (k==9) EMIT2("orv",s,q);
						else {error(21); goto stop;}
					}
					q=s;  freeTemp(c,r);
				}
				if (b!='B') EMIT2("sindv",p,q);
				else {
					needMask=true;
					if (c->task!=0) {error(23); goto stop;}
					EMIT2("setv","L1","N-1");
					EMIT2("setv","L2",p);
					EMIT1("calls", " 7");   //true.false,load rhs
					if (strcmp(q,"N1")==0) {
						EMIT2("orv",name,"L1");
					} else if (strcmp(q,"N0")==0) {
						EMIT2("andv",name,"L2");
					} else {
						EMIT2("andv",name,"L2");
						if (!iscBoolParameter(c)&&(c->cons!=C_RESULT)) {char temp4[8],temp5[8],*ps;
							setBooleanBase(c->val,temp4,&m);
							ps=getTemp(c);
							if (ps==NULL) goto stop;
							q=getName(c,ps);
							EMIT2("setv",q,temp4);
							temp5[0]='N'; itoa((1<<m),temp5+1,10);
							EMIT2("andv",q,temp5);
							if (m) EMIT2("divv",q,temp5);
							freeTemp(c,ps);
						}
						EMIT2("mulv",q,"L1");
						EMIT2("orv",name,q);
					}
				}
				if (isTemp(c->val)) freeTemp(c,c->val);
				if (isTemp(js)) freeTemp(c,js);
				continue;
			}
			n=atoi(c->val+1);
			if (n<0) {error2(22,c->val+1); goto stop;}
			if (isParameterMatrix(js)) {
				if (n!=0) {
					js=getTemp(c);
					if (js==NULL) goto stop;
					p=getName(c,js);
					EMIT2("setv",p,c->val);
					EMIT2("sumv",p,name);
				} else p=name;
				k=cass(s+i);  goto mo;
			}
			if (b!='B') {
				if (name[0]=='G') {
					if (n>30) {error2(25,c->val+1); goto stop;}
				} else if (n>14) {error2(26,c->val+1); goto stop;}
				j+=(name[0]!='T')?n:-n;
				itoa(j,name+1+(b=='F'),10);
			} else {
				if (n>=15) {error2(24,c->val+1); goto stop;}
				itoa(j*100+n,name+2,10);
			}
			if (b!='I') name[1]=b;
			js=&name[0];
		}
		k=cass(s+i);
		if ((k==1)||(k==2)) {
			if (m!=c->nNames) {error(21); goto stop;}
			if (isBool(js)) {error(27); goto stop;}
			if (isFloat(js)) EMIT2("sumv",js,(k==1)?"F1":"F-1");
			else EMIT2("sumv",js,(k==1)?"N1":"N-1");
			i+=2;
			continue;
		}
		if (k!=0) {char *ps,*pps,*qs,*qqs;
			if (m!=c->nNames) {error(21); goto stop;}
			*ii=i+2;
			failure=expr(s,ii,ls,c); i=*ii;
			if (failure!=M_NOERR) goto stop;
			if (k==10) {
				ps=getTemp(c);
				if (ps==NULL) goto stop;
				pps=getName(c,ps);
				qs=getTemp(c);
				if (qs==NULL) goto stop;
				qqs=getName(c,qs);
			}
			if (isFloat(js)&&!iscFloat(c)) {error(18); goto stop;}
			if (isInteger(js)&&!iscInteger(c)) {error(18); goto stop;}
			if (isBool(js)&&!iscBool(c)) {error(18); goto stop;}
			if (iscBool(c)) {char temp2[8],temp3[8];
				if ((k>=3)&&(k<=7)) {error(28); goto stop;}
				if (k==10) {error(29); goto stop;}
				if (isBoolParameter(js)) {
					j=0;
					strcpy(temp,js);
				} else {
					setBooleanBase(js,temp,&j);
				}
				temp2[0]='N'; temp3[0]='N';
				itoa(~(1<<j),temp2+1,10);  itoa((1<<j),temp3+1,10);
				if (isBool(c->cons)) {
					m=atoi(c->val+1);
					if (m==0) {
						if (k==9) continue;
						else EMIT2("andv",temp,temp2);
					} else if (k==8) continue;
					else EMIT2("orv",temp,temp3);
				} else {char *ps,*pps,temp4[4],temp5[8];
yyy:
					if (iscBoolParameter(c)) {
						pps=c->val; m=0;
						if (j!=0) {
							ps=getTemp(c);
							if (ps==NULL) goto stop;
							pps=getName(c,ps);
							EMIT2("setv",pps,c->val);
							freeTemp(c,ps);
						}
					} else if (!isTemp(c->val)) {
						setBooleanBase(c->val,temp4,&m);
						ps=getTemp(c);
						if (ps==NULL) goto stop;
						pps=getName(c,ps);
						EMIT2("setv",pps,temp4);
						temp5[0]='N'; itoa((1<<m),temp5+1,10);
						EMIT2("andv",pps,temp5);
						freeTemp(c,ps);
					} else {pps=getName(c,c->val); freeTemp(c,c->val);  m=0;}			
					if (j<m) {
						temp5[0]='N'; itoa((1<<(m-j)),temp5+1,10);
						EMIT2("divv",pps,temp5);
					} else if (j>m) {
						temp5[0]='N'; itoa((1<<(j-m)),temp5+1,10);
						EMIT2("mulv",pps,temp5);
					}
					if (k==8) {
						temp2[0]='N'; itoa(~(1<<j),temp2+1,10);
						//EMIT2("orv",pps,temp2);
						EMIT2("andv",temp,pps);
					} else EMIT2("orv",temp,pps);
				}
				continue;
			}
			if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
			if (k==3) EMIT2("sumv",js,c->val);
			if (k==4) EMIT2("subv",js,c->val);
			if (k==5) {
				if (isFloat(js)) {
					if (isNumber(c->cons)&&(c->val[2]=='.')&&(c->val[3]=='0')&&(c->val[4]=='\0')) {
						c->val[0]='N'; c->val[2]='\0';
						EMIT2("mulv",js,c->val);
						continue;
					}
					EMIT2("mulv",js,c->val);
					EMIT2("sumv",js,"F0.5");
					EMIT2("divv",js,"F1");
				} else EMIT2("mulv",js,c->val);
			}
			if (k==6) {
				if (divide(js,c)!=M_NOERR) goto stop;
			}
			if (k==7) {
				if (isFloat(js)) {error(19); goto stop;}
				if (isNumber(c->cons)) {float jj;
					jj=(float)atof(c->val+1);
					if (jj==0) {error(30); goto stop;}
				}
				if (isNumber(c->cons)&&((j=pow2(c->val))!=0)) {
					itoa((1<<j)-1,c->val+1,10);
					EMIT2("andv",js,c->val);
				} else {
					ps=getTemp(c);
					if (ps==NULL) goto stop;
					pps=getName(c,ps);
					EMIT2("setv",pps,js);
					EMIT2("divv",pps,c->val); turnOff(c->val);
					EMIT2("mulv",pps,c->val);
					EMIT2("subv",js,pps);
					freeTemp(c,ps);
				}
			}
			if (k==8) {
				if (isFloat(js)) {error(20); goto stop;}
				EMIT2("andv",js,c->val);
			}
			if (k==9) {
				if (isFloat(js)) {error(20); goto stop;}
				EMIT2("orv",js,c->val);
			}
			if (k==10) {
				if (isFloat(js)) {error(31); goto stop;}
				freeTemp(c,ps);  freeTemp(c,qs);
				if (isNumber(c->cons)) {int x;
					x=atoi(c->val+1);
					sprintf(qqs,"N%d",~x);
					EMIT2("setv",pps,"N-1");
					EMIT2("subv",pps,js);
					EMIT2("orv",pps,qqs);
					EMIT2("orv",js,c->val);
					EMIT2("andv",js,pps);
					continue;
				}
				EMIT2("setv",pps,"N-1");
				EMIT2("setv",qqs,"N-1");
				EMIT2("subv",pps,js);  
				EMIT2("subv",qqs,c->val);  turnOff(c->val);
				EMIT2("orv",pps,qqs);
				EMIT2("orv",js,c->val);
				EMIT2("andv",js,pps);
			}
			continue;
		}
		if (s[i]=='=') {
			*ii=i+1;
xxx:		failure=expr(s,ii,ls,c); i=*ii;
			if (failure!=M_NOERR) goto stop;
			if (isFloat(js)&&!iscFloat(c)) {error(18); goto stop;}
			if (isInteger(js)&&!iscInteger(c)) {error(18); goto stop;}
			if (isBool(js)&&!iscBool(c)) {error(18); goto stop;}
			if (iscBool(c)) {char temp2[8];
				setBooleanBase(js,temp,&j);
				temp2[0]='N';
				if (isBoolParameter(js)) {
					if (iscBoolParameter(c)) {
						EMIT2("setv",js,c->val);
					} else if (isResult(c->cons)) {char *pps;
						pps=getName(c,c->val); freeTemp(c,c->val);
						EMIT2("setv",js,pps);
					} else if (isBool(c->cons)) {
						k=atoi(c->val+1);
						EMIT2("setv",js,(k==0)?"N0":"N1");
					} else {
						EMIT2("setv",js,temp);
						j=atoi(c->val+2);
						if (j!=0) {itoa((1<<j),temp2+1,10); EMIT2("divv",js,temp2);}
						EMIT2("andv",js,"N1");
					}
					continue;
				}
				itoa(~(1<<j),temp2+1,10);
				if (isBool(c->cons)) {
					k=atoi(c->val+1);
					if (k==0) EMIT2("andv",temp,temp2);
					else {itoa((1<<j),temp2+1,10); EMIT2("orv",temp,temp2);}
				} else {
					EMIT2("andv",temp,temp2);
					k=9;
					goto yyy;
				}
			} else {
				if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
				EMIT2("setv",js,c->val);
			}
			continue;
		}
	}
	error(32);
	goto stop;
	int isFunc;
zz: if (forstate) {error(204); goto stop;}
	switch (j) {int r;
    case 2:
		if (W(s,&i,ls,&c->lineno)) goto stop;
		if ((s[i]=='(')&&(s[i+1]!=';')) {
			i++;
			if (W(s,&i,ls,&c->lineno)) goto stop;
			forstate=1;
			if (isalpha(s[i])) {goto zz22;}
			if (s[i]==';') continue;
			error(203); goto stop;
		}
		if ((s[i]=='(')&&(s[i+1]==';')&&(s[i+2]==';'))
			if (s[i+3]==')') {i+=4; js=NULL;; goto zz2;}
			else {error(209); goto stop;}
		if ((s[i]!='(')||(s[i+1]!=';')||(s[i+2]!='-')||(s[i+3]!='-')) {error(33); goto stop;}
		i+=4;
		m=strspn(s+i,"abcdefghijkl_mnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		if (m<=0) {error(34); goto stop;}
		if (m>35) {error2(8,name); goto stop;}
		strncpy(name,s+i,m);
		name[m]=0; i+=m;
		js=findName(c,name);
		if (js==NULL) {error2(35,name); goto stop;}
		if (!isWritable(js)) {error2(36,name); goto stop;}
		if (isBool(js)) {error2(37,name); goto stop;}
		if ((s[i]!='>')||(s[i+1]!='=')||(s[i+2]!='0')||(s[i+3]!=';')||(s[i+4]!=')')) {error(33); goto stop;}
		i+=5;
zz2:	if (W(s,&i,ls,&c->lineno)) goto stop;
		if (s[i]!='{') {error(38); goto stop;}
		i++;
		if (js!=NULL)
			for (j=0; j<iffor; j++)
				if (iffortype[j]==js) {error2(39,name); goto stop;}
		j=getLabel()*10000+getLabel();
		ifforaddr[iffor]=j;
		iffortype[iffor]=js; iffor++;
		EMIT(strcat(makeLabel(j%10000),":"));
		if (js==NULL) continue;
		if (isFloat(js)) {
			EMIT4("chkl","F0","1",js,makeLabel(j/10000));
			EMIT2("subv",js,"F1");
		} else EMIT2("decvjnl",js,makeLabel(j/10000));
		continue;
	case 3: //break
	case 4: //continue
		for (m=iffor-1; m>=0; m--)
			if (ifforaddr[m]>0) break;
		if (m<0) {error(40); goto stop;}
		if (j==3) EMIT1("jmpl",makeLabel(ifforaddr[m]/10000));
		else EMIT1("jmpl",makeLabel(ifforaddr[m]%10000));
		continue;
	case 5: //if
		j=getLabel()*10000+getLabel();
zif:    if (W(s,&i,ls,&c->lineno)) goto stop;
		if (condtest(s,&i,ls,c,j)) goto stop;
		ifforaddr[iffor]=-j;
		iffortype[iffor]=NULL; iffor++;
		if (s[i++]!='{') {error(45); goto stop;}
		continue;
	case 6:
		{Name names[6]; char ops[16];
		js=strchr(s+i,' ');
		if (js==NULL) {error(46); goto stop;}
		j=js-s;
		if ((j-i)>=15) {error(47); goto stop;}
		strncpy(ops,s+i,j-i); ops[j-i]=0;
		i+=j-i;
		if (W(s,&i,ls,&c->lineno)) goto stop;
		if (s[i]!='{') {error(48); goto stop;}
		*ii=i+1;
			for(j=0;j<=6;j++) {
				if (W(s,ii,ls,&c->lineno)) goto stop;
				if (j>0)
					if (s[*ii]==',') *ii+=1;
				if (s[*ii]=='}') {
					*ii+=1;
					switch (j) {
					case 1: EMIT(names[0]); break;
					case 2: EMIT1(names[0],names[1]); break;
					case 3: EMIT2(names[0],names[1],names[2]); break;
					case 4: EMIT3(names[0],names[1],names[2],names[3]); break;
					case 5: EMIT4(names[0],names[1],names[2],names[3],names[4]); break;
					default: error(49); goto stop;
					}
					break;
				}
				if ((s[*ii]=='(')&&(s[*ii+1]==')')) {error(199); goto stop;}
				failure=expr(s,ii,ls,c);
				if (failure!=M_NOERR) goto stop;
				if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
				if (ops[j*2+1]=='C') {
					if ((c->cons!=C_STRING)&&(c->cons!=C_INTEGER)) {error3(50,j+1); goto stop;}
					strcpy(names[j],c->val+1);
					continue;
				}
				if (ops[j*2+1]=='I') {
					if (c->type!=C_INTEGER) {error3(51,j+1); goto stop;}
					//if (c->cons==C_INTEGER) strcpy(names[j++],"2");
					//else strcpy(names[j++],"0");
					strcpy(names[j],c->val);
					continue;
				}
				if (ops[j*2+1]=='V') {
					if (((c->cons!=C_VARIABLE)&&(c->cons!=C_RESULT))||((c->val[0]!='T')&&(c->val[0]!='L')&&(c->val[0]!='G')&&(c->val[0]!='$'))) {error3(52,j+1); goto stop;}
					strcpy(names[j],c->val);
					continue;
				}
				if (ops[j*2+1]=='G') {
					if ((c->cons!=C_VARIABLE)||(c->val[0]!='G')) {error3(53,j+1); goto stop;}
					strcpy(names[j],c->val);
					continue;
				}
				if (ops[j*2+1]=='T') {
					if (!isInteger(c->cons)&&!isTask(c->val)) {error3(54,j+1); goto stop;}
					strcpy(names[j],c->val+1);
					continue;
				}
				if (ops[j*2+1]=='H') {
					if ((c->cons!=C_VARIABLE)||((c->val[0]!='G')&&(c->val[0]!='#'))) {error3(55,j+1); goto stop;}
					strcpy(names[j],c->val);
					continue;
				}
				error(198);
				goto stop;
			}
		}
		i=*ii;
		continue;
	case 7:
		if ((iffor==0)&&(c->task==0)) {
			EMIT("endt");
			c->task=-1;
			for(k=0;k<8;k++) {
				if (tref[k]&&((smax[k]+tempcnt)>16)) {error2(201,subs[k]); goto stop;}
			}
			maxTask=max(maxTask,tempcnt);
			freeLocals(c);
			tempcnt=1;  tempmask=1; taskin--;
			continue;
		}
		if ((iffor==0)&&(c->task==1)) {
			EMIT("ends");
			if (isReturn==1) {error(200); goto stop;}
			smax[subCnt-1]=tempcnt;
			maxLocal=max(maxLocal,tempcnt);
			freeLocals(c);
			tempcnt=1;  tempmask=1;  subin--;
			c->task=-1;
			continue;
		}
		if (iffor<=0) {error(1); goto stop;}
		if (W(s,&i,ls,&c->lineno)) goto stop; 
		iffor--;
		if (ifforaddr[iffor]>0) {
			EMIT1("jmpl",makeLabel(ifforaddr[iffor]%10000));
			EMIT(strcat(makeLabel(ifforaddr[iffor]/10000),":"));
		} else {
			j=-ifforaddr[iffor];
			if ((s[i]=='e')&&(s[i+1]=='l')&&(s[i+2]='s')&&(s[i+3]=='e')) {
				i+=4;
				if (W(s,&i,ls,&c->lineno)) goto stop;
				EMIT1("jmpl",makeLabel(j%10000));
				EMIT(strcat(makeLabel(j/10000),":"));
				if ((s[i]=='i')&&(s[i+1]=='f')) {
					j=getLabel()*10000+j%10000;
					i+=2;
					goto zif;
				} else if (s[i]!='{') {error(56); goto stop;}
				ifforaddr[iffor]=-(j%10000);
				i++;  iffor++;
				continue;
			}
			if ( (j/10000)!=0)EMIT(strcat(makeLabel(j/10000),":"));
			EMIT(strcat(makeLabel(j%10000),":"));
		}
		continue;
	case 8:
		if (c->task!=1) {error(57); goto stop;}
		if (W(s,&i,ls,&c->lineno)) goto stop;
		j=getSub(c,subCnt-1,k,m,m);
		if (s[i]!=';') {
			if (j==0) {error(58); goto stop;}
			isReturn++;
			strcpy(name,(j==1)?"L1":(j==2)?"LX1":"LF1");
			js=&name[0];
			kludge=true; *ii=i;
			goto xxx;
		} else {
			if (j) {error(59); goto stop;}
		}
		EMIT("rets");
		continue;
	case 14: b++;
	case 13: b++;
	case 17:
	case 9:
		for (j=0;;j++) {
			b=b%100;
			if (W(s,&i,ls,&c->lineno)) goto stop;
			if (((s[i]>='a')&&(s[i]<='z'))||((s[i]>='A')&&(s[i]<='Z'))) {
				m=strspn(s+i,"abcdefg_hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
				strncpy(name,s+i,m); name[m]=0;
				i+=m;
				if (W(s,&i,ls,&c->lineno)) goto stop;
				if (m>35) {error2(60,name); goto stop;}
				if ((js=findName(c,name))!=NULL) {error2(61,name); goto stop;}
				if ((j==0)&&(s[i]=='[')&&(s[i+1]==']')) {
					i+=2;
					if (W(s,&i,ls,&c->lineno)) goto stop;
					if (s[i]!='=') {error2(62,name); goto stop;}
					i++;
					if (W(s,&i,ls,&c->lineno)) goto stop;
					arrays[arraycnt][0]=0;
					if ((b==0)&&((s[i]=='"')||(s[i]=='\''))) {char *ss; char t2[6];
						k=factor(s, &i, ls, c);
						if (k!=M_NOERR) goto stop;
						if (c->cons!=C_STRING) {error2(63,name); goto stop;}
						ss=c->val+1;
						t2[0]='N';
						for (j=0;*ss;ss++,j++) {
							itoa(*ss,t2+1,10);
							if (j!=0) strcat(arrays[arraycnt],",");
							strcat(arrays[arraycnt],t2);
						}
						goto s21;
					}
					if (s[i]!='{') {error2(64,name); goto stop;}
					i++;
					for (j=0;;i++) {
						if (W(s,&i,ls,&c->lineno)) goto stop;
						k=expr(s, &i, ls, c);
						if (k!=M_NOERR) goto stop;
						if (((b==0)||(b==3))&&(c->cons!=C_INTEGER)) {error3(65,j+1); goto stop;}
						if ((b==2)&&(c->cons!=C_FLOAT)) {error3(66,j+1); goto stop;}
						if ((b==1)&&(c->cons!=C_BOOL)) {error3(67,j+1); goto stop;}
						if (j!=0) strcat(arrays[arraycnt],",");
						strcat(arrays[arraycnt],c->val);
						j++;
						if (W(s,&i,ls,&c->lineno)) goto stop;
						if (s[i]=='}') {i++; break;}
						if (s[i]!=',') {error(68); goto stop;}
					}
s21:				arrayn[arraycnt]=j;
					arraycnt++;
					if (defineName(c,name,4+(b==2)+((b==1)*2))==0) goto stop;
					isConst=false;
					break;
				}
				if (isConst) {error(69); goto stop;}
				if ((j==0)&&(s[i]=='(')) {isFunc=b+1; isReturn=1; goto sss;}
				if ((j==0)&&(s[i]=='[')&&(s[i+1]!=']')) {
					i++;
					k=expr(s, &i, ls, c);
					if (k!=M_NOERR) goto stop;
					if (c->cons!=C_INTEGER) {error2(70,name); goto stop;}
					if (s[i]==',') {error2(74,name); goto stop;}
					if (s[i++]!=']') {error2(71,name); goto stop;}
					k=atoi(c->val+1);
					if (k<=1) {error2(72,name); goto stop;}
					if ((b==1)&&(k>15)) {error2(73,name); goto stop;}
					b+=k*100;
				}
				if (defineName(c,name,b)==0) goto stop;
				if (s[i]==',') i++;
				else break;
			} else {error(75); goto stop;}
		}
		continue;
	case 10:
		if (c->task>=0) {error(76); goto stop;}
		if (W(s,&i,ls,&c->lineno)) goto stop;
		m=strspn(s+i,"abcdefg_hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		if (m<=0) {error(78); goto stop;}
		strncpy(name,s+i,m); name[m]=0;
		if (m>35) {error2(77,name); goto stop;}
		i+=m;
		if ((js=findName(c,name))!=NULL) {error2(79,name); goto stop;}
		if (strcmp(name,"main")==0) {j=0; gotmain++;}
		else j=++taskCnt;
		if (makeTask(c,name,j)==0) goto stop;
		if (W(s,&i,ls,&c->lineno)) goto stop;
		if (s[i++]!='(') {error2(80,name); goto stop;}
		if (W(s,&i,ls,&c->lineno)) goto stop;
		if (s[i++]!=')') {error2(81,name); goto stop;}
		if (W(s,&i,ls,&c->lineno)) goto stop;
		if (s[i++]!='{') {error2(82,name); goto stop;}
		c->task=0; taskin++;
		continue;
	case 11:
		if (W(s,&i,ls,&c->lineno)) goto stop;
		if (s[i]!='(') {error(83); goto stop;}
		j=getLabel()*10000+getLabel();
		EMIT(strcat(makeLabel(j%10000),":"));
		failure=expr(s,&i,ls,c);
		if (failure!=M_NOERR) goto stop;
		if (isTemp(c->val)) {freeTemp(c,c->val); strcpy(c->val,getName(c,c->val));}
		if (W(s,&i,ls,&c->lineno)) goto stop;
		if (s[i]!='{') {error(84); goto stop;}
		i++;		
		ifforaddr[iffor]=j;
		iffortype[iffor]=NULL; iffor++;
		if (iscConstant(c)) {float ii;
			ii=(float)atof(c->val+1);
			if (ii==0) EMIT1("jmpl",makeLabel(j/10000));
			continue;
		}
		if (iscBool(c)&&isVariable(c->cons)&&!iscBoolParameter(c)) {char temp4[4],*ps,*pps; char temp5[8];
			setBooleanBase(c->val,temp4,&m);
			ps=getTemp(c);
			if (ps==NULL) goto stop;
			strcpy(c->val,pps=getName(c,ps));
			EMIT2("setv",pps,temp4);
			temp5[0]='N'; itoa((1<<m),temp5+1,10);
			EMIT2("andv",pps,temp5);
			freeTemp(c,ps);
		}
		EMIT4("chkl",c->val,"3",iscFloat(c)?"F0":"N0",makeLabel(j/10000));
		continue;
	case 12:
		if (W(s,&i,ls,&c->lineno)) goto stop;
		if (((s[i]>='a')&&(s[i]<='z'))||((s[i]>='A')&&(s[i]<='Z'))) {
			m=strspn(s+i,"abcdefg_hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
			strncpy(name,s+i,m); name[m]=0;
			i+=m;
			if (W(s,&i,ls,&c->lineno)) goto stop;
			if (m>35) {error2(85,name); goto stop;}
			if ((js=findName(c,name))!=NULL) {error2(86,name); goto stop;}
			isFunc=0;
sss:		if (c->task>=0) {error2(87,name); goto stop;}
			subin++;
			if (makeSub(c,name)==0) goto stop;
			if (s[i++]!='(') {error2(88,name); goto stop;}
			if (W(s,&i,ls,&c->lineno)) goto stop;
			j=0; k=0; c->task=1;  r=0;
			for (;;) {
				if (strncmp(s+i,"short ",6)==0) {b=0; i+=6;}
				else if (strncmp(s+i,"int ",4)==0) {b=0; i+=4;}
				else if (strncmp(s+i,"char ",5)==0) {b=0; i+=5;}
				else if (strncmp(s+i,"bool ",5)==0) {b=3; i+=5;}
				else if (strncmp(s+i,"float ",6)==0) {b=2; i+=6;}
				else if (strncmp(s+i,"const ",6)==0) {error(94); goto stop;}
				else break;
				j|=b<<(k+k);
				if (W(s,&i,ls,&c->lineno)) goto stop;
				m=strspn(s+i,"abcdefg_hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
				if (m<=0) {error3(89,k+1); goto stop;}
				strncpy(name,s+i,m); name[m]=0;
				if (m>35) {error2(90,name); goto stop;}
				i+=m;
				if ((js=findName(c,name))!=NULL) {error2(91,name); goto stop;}
				if (s[i]=='[') {
					i++;
					if (s[i++]!=']') {error2(92,name); goto stop;}
					if (b==3) {error2(93,name); goto stop;}
					b+=9999900;
					r|=(1<<k);
				}
				if (defineName(c,name,b)==0) goto stop;
				if (W(s,&i,ls,&c->lineno)) goto stop;
				k++;
				if (s[i]!=',') break;
				i++;
				if (W(s,&i,ls,&c->lineno)) goto stop;
			}
			if (s[i++]!=')') {error(95); goto stop;}
			if (W(s,&i,ls,&c->lineno)) goto stop;
			if (s[i++]!='{') {error(202); goto stop;}
			if (k>8) {error(96); goto stop;}
			setSub(c,j,k,r,isFunc);
			continue;
		}
	case 15:
		if (W(s,&i,ls,&c->lineno)) goto stop;
		m=strspn(s+i,"abcdefg_hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		if (m<=0) {error(97); goto stop;}
		strncpy(name,s+i,m); name[m]=0;
		if (m>35) {error2(98,name); goto stop;}
		i+=m;
		if ((js=findName(c,name))==NULL) {error2(99,name); goto stop;}
		if (!isTask(js)) {error2(100,name); goto stop;}
		EMIT1("start",js);
		continue;
	case 16:
		if (W(s,&i,ls,&c->lineno)) goto stop;
		m=strspn(s+i,"abcdefg_hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		if (m<=0) {error(101); goto stop;}
		strncpy(name,s+i,m); name[m]=0;
		if (m>35) {error2(102,name); goto stop;}
		i+=m;
		if ((js=findName(c,name))==NULL) {error2(103,name); goto stop;}
		if (!isTask(js)) {error2(104,name); goto stop;}
		EMIT1("stop",js);
		continue;
	} //switch
	error(105); goto stop;
	} /*for*/
	goto ok;
}
static void C(char *s) {
	int i;
	if (s[0]=='\0') return;
	assert(s[1]!='B');
	if (s[1]=='F') s[1]=' ';
	if (s[0]=='L') i=atoi(s+((s[1]=='X')?2:1))+32;
	else if (s[0]=='G') i=atoi(s+1);
	else if (s[0]=='T') i=47-atoi(s+1)+1;
	else if (s[0]=='Z') i=atoi(s+1);
	else if (s[0]=='P') i=atoi(s+1);
	else if (s[0]=='N') i=atoi(s+1);
	else return;
	itoa(i,s,10);
}
FILE *output=stdout;
static void C1(char *s) {
	int i;
	if (s[0]==0) return;
	assert(s[1]!='B');
	if (s[1]=='F') s[1]=' ';
	if (s[0]=='L') {
		i=atoi(s+((s[1]=='X')?2:1))+32;
	} else if (s[0]=='G') i=atoi(s+1);
	else if (s[0]=='T') i=47-atoi(s+1)+1;
	else if (s[0]=='N') {
		i=atoi(s+1);
		if ((i>32767)||(i<-32768)) {
			fprintf(output,"\ninteger %d too large\n",i); exit(-1);
		}
	} else if (s[0]=='F') {float j=(float)atof(s+1);
		if (j>=0) i=(int)(j*10.0f+0.05f); else i=(int)(j*10.0f-0.05f);
		if ((i>32767)||(i<-32768)) {
			fprintf(output,"\nfloat %d too large\n",i); exit(-1);
		}
	} else if (s[0]=='R') {
		strcpy(s,"4,32767");
		return;
	} else if (s[0]=='E') {
		s[0]=' ';
		return;
	} else if (s[0]=='#') {
		s[0]=' ';  s[1]=' ';
		return;
	} else return;
	if ((s[0]=='N')||(s[0]=='F')) strcpy(s,"2,"); else strcpy(s,"0,");
	itoa(i,s+2,10);
}

long tmps=0;

static int isaTemp(char s[]) {int j;
	j=atoi(s+1);
	return (tmps&(1<<j))!=0;
}
static void optimize(int i) {
	int j;
	if (strcmp(code.code[i].opcode,";ends")==0) tmps=0;
	if (strcmp(code.code[i].opcode,";endt")==0) tmps=0;
	if ((strcmp(code.code[i].opcode,";define")==0)&&(code.code[i].arg1[0]=='$')) {
		j=atoi(code.code[i].arg1+1);
		tmps|=1<<j;
	}
//setv T,N0 chk  setv  T,N1  s: setv V,T-->   setv V,N0 chk  setv  V,N1  s:
	if (strcmp(code.code[i].opcode,"setv")==0) {
		if (isaTemp(code.code[i].arg1)&&(strcmp(code.code[i].arg2,"N0")==0)) {
			if ((strncmp(code.code[i+1].opcode,"chk",3)==0) &&
						(strcmp(code.code[i+1].arg3,code.code[i].arg1)!=0)) {
				if ((strcmp(code.code[i+2].opcode,"setv")==0) &&
						(strcmp(code.code[i+2].arg1,code.code[i].arg1)==0)) {
					if (strchr(code.code[i+3].opcode,':')!=NULL)
						if ((strcmp(code.code[i+4].opcode,"setv")==0) &&
							!isaTemp(code.code[i+4].arg1)&&
							(strcmp(code.code[i+1].arg3,code.code[i+4].arg1)!=0)&&
							(strcmp(code.code[i+1].arg1,code.code[i+4].arg1)!=0)&&
							(strcmp(code.code[i].arg1,code.code[i+4].arg2)==0)) {
							code.code[i+4].opcode[0]=';';
							strcpy(code.code[i].arg1,code.code[i+4].arg1);
							strcpy(code.code[i+2].arg1,code.code[i+4].arg1);
						}
				}
			}
		}
	}
//setv T,... setv  Y,T  -->   setv  Y,...
	if ( (strcmp(code.code[i].opcode,"setv")==0)||
		(strcmp(code.code[i].opcode,"absv")==0)||
		(strcmp(code.code[i].opcode,"sgnv")==0) ) {
		if (strcmp(code.code[i+1].opcode,"setv")==0)
			if (strcmp(code.code[i].arg1,code.code[i+1].arg2)==0) {
				if (isaTemp(code.code[i].arg1)) {
					strcpy(code.code[i+1].opcode,code.code[i].opcode);
					code.code[i].opcode[0]=';';
					strcpy(code.code[i+1].arg2,code.code[i].arg2);
				}
			}
	}
//if (x op y) setv   chk X,?  setv  s?:   chk X,3,N0,s*  --> chk X,?,?,s*
	if (strcmp(code.code[i].opcode,"setv")==0) {
		if (strncmp(code.code[i+1].opcode,"chk",3)==0)
			if (strcmp(code.code[i+2].opcode,"setv")==0)
				if (strchr(code.code[i+3].opcode,':')!=NULL)
					if ((strncmp(code.code[i+4].opcode,"chk",3)==0) &&
						(strcmp(code.code[i+4].arg1,code.code[i+2].arg1)==0)) {
						j=code.code[i+4].arg2[0];
						if ((j=='3')&&(strcmp(code.code[i+4].arg3,"N0")==0)) {
							code.code[i].opcode[0]=';';
							code.code[i+2].opcode[0]=';';
							strcpy(code.code[i+1].arg4,code.code[i+4].arg4);
							strcpy(code.code[i+1].opcode,code.code[i+4].opcode);
							code.code[i+4].opcode[0]=';';
						} else assert(0);
					}
	}
//if (x>=y) setv   chk X,?  setv  s?:   chk X,3,N0,s*  --> chk X,?,?,s*
	if (strcmp(code.code[i].opcode,"setv")==0) {
		if (strncmp(code.code[i+1].opcode,"chk",3)==0)
			if (strncmp(code.code[i+2].opcode,"chk",3)==0)
				if (strchr(code.code[i+3].opcode,':')!=NULL)
					if (strcmp(code.code[i+4].opcode,"setv")==0)
						if (strchr(code.code[i+5].opcode,':')!=NULL)
							if ((strncmp(code.code[i+6].opcode,"chk",3)==0) &&
								(strcmp(code.code[i+6].arg1,code.code[i+4].arg1)==0)) {
									j=code.code[i+6].arg2[0];
									if ((j=='3')&&(strcmp(code.code[i+6].arg3,"N0")==0)) {
										code.code[i].opcode[0]=';';
										code.code[i+4].opcode[0]=';';
										code.code[i+5].opcode[0]=';';
										strcpy(code.code[i+2].arg4,code.code[i+6].arg4);
										strcpy(code.code[i+2].opcode,code.code[i+6].opcode);
										code.code[i+6].opcode[0]=';';
									} else assert(0);
							}
	}
//if (x op y)&&(x op y) setv   chk X,?  setv  s?:   chk X,3,N0,s*  --> chk X,?,?,s*
	if (strcmp(code.code[i].opcode,"setv")==0) {
		if (strncmp(code.code[i+1].opcode,"chk",3)==0)
			if (strcmp(code.code[i+2].opcode,"setv")==0)
				if (strchr(code.code[i+3].opcode,':')!=NULL)
					if (strcmp(code.code[i+4].opcode,";define")==0)
						if ((strcmp(code.code[i+5].opcode,"setv")==0)&&(strcmp(code.code[i+5].arg2,"N0")==0))
							if (strncmp(code.code[i+6].opcode,"chk",3)==0)
								if (strcmp(code.code[i+7].opcode,";define")==0)
									if (strcmp(code.code[i+8].opcode,"setv")==0)
										if (strncmp(code.code[i+9].opcode,"chk",3)==0)
											if (strcmp(code.code[i+10].opcode,"setv")==0)
												if (strchr(code.code[i+11].opcode,':')!=NULL)
													if (strcmp(code.code[i+12].opcode,"setv")==0)
														if (strchr(code.code[i+13].opcode,':')!=NULL)
														if ((strncmp(code.code[i+14].opcode,"chk",3)==0) &&
															(strcmp(code.code[i+14].arg1,code.code[i+5].arg1)==0)) {
															j=code.code[i+14].arg2[0];
															if ((j=='3')&&(strcmp(code.code[i+14].arg3,"N0")==0)) {
																code.code[i].opcode[0]=';';
																code.code[i+2].opcode[0]=';';
																code.code[i+5].opcode[0]=';';
																code.code[i+6].opcode[0]=';';
																code.code[i+8].opcode[0]=';';
																code.code[i+10].opcode[0]=';';
																code.code[i+12].opcode[0]=';';
																strcpy(code.code[i+1].arg4,code.code[i+14].arg4);
																strcpy(code.code[i+9].arg4,code.code[i+14].arg4);
																strcpy(code.code[i+1].opcode,code.code[i+14].opcode);
																strcpy(code.code[i+9].opcode,code.code[i+14].opcode);
																code.code[i+14].opcode[0]=';';
															} else assert(0);
														}
	}
//if (x op y)||(x op y) setv   chk X,?  setv  s?:   chk X,3,N0,s*  --> chk X,?,?,s*
	if (strcmp(code.code[i].opcode,"setv")==0) {
		if (strncmp(code.code[i+1].opcode,"chk",3)==0)
			if (strcmp(code.code[i+2].opcode,"setv")==0)
				if (strchr(code.code[i+3].opcode,':')!=NULL)
					if (strcmp(code.code[i+4].opcode,";define")==0)
						if ((strcmp(code.code[i+5].opcode,"setv")==0)&&(strcmp(code.code[i+5].arg2,"N1")==0))
							if (strncmp(code.code[i+6].opcode,"chk",3)==0)
								if (strcmp(code.code[i+7].opcode,";define")==0)
									if (strcmp(code.code[i+8].opcode,"setv")==0)
										if (strncmp(code.code[i+9].opcode,"chk",3)==0)
											if (strcmp(code.code[i+10].opcode,"setv")==0)
												if (strchr(code.code[i+11].opcode,':')!=NULL)
													if (strcmp(code.code[i+12].opcode,"setv")==0)
														if (strchr(code.code[i+13].opcode,':')!=NULL)
														if ((strncmp(code.code[i+14].opcode,"chk",3)==0) &&
															(strcmp(code.code[i+14].arg1,code.code[i+5].arg1)==0)) {
															j=code.code[i+14].arg2[0];
															if ((j=='3')&&(strcmp(code.code[i+14].arg3,"N0")==0)) {
																code.code[i].opcode[0]=';';
																code.code[i+2].nArgs=1;
																strcpy(code.code[i+2].opcode,"jmpl");
																strcpy(code.code[i+2].arg1,code.code[i+9].arg4);
																code.code[i+5].opcode[0]=';';
																code.code[i+6].opcode[0]=';';
																code.code[i+8].opcode[0]=';';
																code.code[i+10].opcode[0]=';';
																code.code[i+12].opcode[0]=';';
																strcpy(code.code[i+9].arg4,code.code[i+14].arg4);
																strcpy(code.code[i+9].opcode,code.code[i+14].opcode);
																code.code[i+14].opcode[0]=';';
															} else assert(0);
														}
	}
//if (b&&c) setv   ;define setv andv chk ;define setv andv chk setv s: chk  --> 
	if ((strcmp(code.code[i].opcode,"setv")==0)&&(strcmp(code.code[i].arg2,"N0")==0)) {
		if (strcmp(code.code[i+1].opcode,";define")==0)
			if (strcmp(code.code[i+2].opcode,"setv")==0)
				if (strcmp(code.code[i+3].opcode,"andv")==0)
					if (strncmp(code.code[i+4].opcode,"chk",3)==0)
						if (strcmp(code.code[i+5].opcode,";define")==0)
							if (strcmp(code.code[i+6].opcode,"setv")==0)
								if (strcmp(code.code[i+7].opcode,"andv")==0)
									if (strncmp(code.code[i+8].opcode,"chk",3)==0)
										if (strcmp(code.code[i+9].opcode,"setv")==0)
											if (strchr(code.code[i+10].opcode,':')!=NULL)
												if ((strncmp(code.code[i+11].opcode,"chk",3)==0) &&
													(strcmp(code.code[i+11].arg1,code.code[i].arg1)==0)) {
													j=code.code[i+11].arg2[0];
													if ((j=='3')&&(strcmp(code.code[i+11].arg3,"N0")==0)) {
														code.code[i].opcode[0]=';';
														code.code[i+9].opcode[0]=';';
														strcpy(code.code[i+4].arg4,code.code[i+11].arg4);
														strcpy(code.code[i+8].arg4,code.code[i+11].arg4);
														strcpy(code.code[i+4].opcode,code.code[i+11].opcode);
														strcpy(code.code[i+8].opcode,code.code[i+11].opcode);
														code.code[i+11].opcode[0]=';';
													} else assert(0);
												}
	}
//if (b||c) setv   ;define setv andv chk ;define setv andv chk setv s: chk  --> 
	if ((strcmp(code.code[i].opcode,"setv")==0)&&(strcmp(code.code[i].arg2,"N1")==0)) {
		if (strcmp(code.code[i+1].opcode,";define")==0)
			if (strcmp(code.code[i+2].opcode,"setv")==0)
				if (strcmp(code.code[i+3].opcode,"andv")==0)
					if (strncmp(code.code[i+4].opcode,"chk",3)==0)
						if (strcmp(code.code[i+5].opcode,";define")==0)
							if (strcmp(code.code[i+6].opcode,"setv")==0)
								if (strcmp(code.code[i+7].opcode,"andv")==0)
									if (strncmp(code.code[i+8].opcode,"chk",3)==0)
										if (strcmp(code.code[i+9].opcode,"setv")==0)
											if (strchr(code.code[i+10].opcode,':')!=NULL)
												if ((strncmp(code.code[i+11].opcode,"chk",3)==0) &&
													(strcmp(code.code[i+11].arg1,code.code[i].arg1)==0)) {
													j=code.code[i+11].arg2[0];
													if ((j=='3')&&(strcmp(code.code[i+11].arg3,"N0")==0)) {
														code.code[i].opcode[0]=';';
														code.code[i+9].nArgs=1;
														if (code.code[i+11].opcode[3]=='l') strcpy(code.code[i+9].opcode,"jmp");
														else strcpy(code.code[i+9].opcode,"jmpl");
														strcpy(code.code[i+9].arg1,code.code[i+11].arg4);
														code.code[i+9].arg2[0]='\0';
														code.code[i+11].opcode[0]=';';
													} else assert(0);
												}
	}
	if (strcmp(code.code[i].opcode,"indv")==0) {
		if ((code.code[i+1].nArgs==2)&&(strcmp(code.code[i].arg1,code.code[i+1].arg2)==0)) {
			strcpy(code.code[i].opcode,code.code[i+1].opcode);
			strcpy(code.code[i].arg3,code.code[i].arg1);
			strcpy(code.code[i].arg1,code.code[i+1].arg1);
			strcpy(code.code[i].arg2,"36");
			code.code[i+1].opcode[0]=';';
			code.code[i].nArgs=3;
			return;
		}
		strcpy(code.code[i].opcode,"setv");
		code.code[i].nArgs=3;
		strcpy(code.code[i].arg2,"36");
		strcpy(code.code[i].arg3,code.code[i].arg1);
	}
	if (strcmp(code.code[i].opcode,"sindv")==0) {
		strcpy(code.code[i].opcode,"set");
		code.code[i].nArgs=3;
		strcpy(code.code[i].arg3,code.code[i].arg2);
		strcpy(code.code[i].arg2,code.code[i].arg1);
		strcpy(code.code[i].arg1,"36");
	}
	if (strcmp(code.code[i].opcode,"addav")==0) {
		code.code[i].nArgs=3;
		strcpy(code.code[i].arg3,code.code[i].arg2);
		strcpy(code.code[i].arg2,"2");
	}
	if (strcmp(code.code[i].opcode,"setv")==0) {
		if (strcmp(code.code[i].arg1,"L1")==0) {
			if (strcmp(code.code[i].arg2,"L1")==0) {
				code.code[i].opcode[0]=';';
			}
		}
	}
}
static int assemble=1;
static void S(char *str) {
	int i=strlen(str);
	char cons[4];
	if (i==0) return;
	if (str[i-1]=='+') {
		str[i-1]=' ';
		fprintf(output,"\tsumv\t");
		i++;
	} else if (str[i-1]=='-') {
		str[i-1]=' ';
		fprintf(output,"\tsubv\t");
		i++;
	} else return;
	if (assemble) {
		if (str[1]=='F') strcpy(cons,"2,10"); else strcpy(cons,"2,1");
		C(str);
	} else if (str[1]=='F') strcpy(cons,"F1"); else strcpy(cons,"N1");
	fprintf(output,"%s,%s\n",str,cons);
}
static void processJumps() {
	int i,j,x,y,cnt=0,top=0;
	int labels[1000],stack[1000];
	for (i=0;i<1000;i++) labels[i]=-1;
	i=code.count; j=0;
	for (;--i>=0;j++) {
		if (code.code[j].nArgs==-1) {
			continue;
		}
		if (code.code[j].opcode[0]=='C') code.code[j].opcode[0]=' ';
		if (code.code[j].opcode[0]==';') continue;
		if ((code.code[j].opcode[0]=='s')&&strchr(code.code[j].opcode,':')) {
			x=atoi(code.code[j].opcode+1);
			assert((x>0)&&(x<1000));
			labels[x]=cnt;
			for (y=0;y<top;y+=3)
				if ((stack[y]==x)&&(cnt<=stack[y+2])) {
					stack[y]=-1;
					if (code.code[stack[y+1]].opcode[0]=='d') {
						code.code[stack[y+1]].opcode[6]=' ';
					} else {
						code.code[stack[y+1]].opcode[3]=' ';
					}
				}
			continue;
		}
		if (strcmp(code.code[j].opcode,"jmpl")==0) {
			x=atoi(code.code[j].arg1+1);
			assert((x>0)&&(x<1000)&&(code.code[j].arg1[0]=='s'));
			if (labels[x]>0) {
				if ((cnt-labels[x])<62) code.code[j].opcode[3]=' ';
				continue;
			}
			cnt+=3;
		} else if (strncmp(code.code[j].opcode,"chk",3)==0) {
			x=atoi(code.code[j].arg4+1);
			assert((x>0)&&(x<1000)&&(labels[x]<0)&&(code.code[j].arg4[0]=='s'));
			cnt+=8;
		} else if (strcmp(code.code[j].opcode,"decvjnl")==0) {
			x=atoi(code.code[j].arg2+1);
			assert((x>0)&&(x<1000)&&(labels[x]<0)&&(code.code[j].arg2[0]=='s'));
			cnt+=4;
		} else {
			cnt+=4;
			continue;
		}
		stack[top++]=x;
		stack[top++]=j;
		stack[top++]=cnt+( (code.code[j].opcode[0]=='c')?254:63 );
	}
}
int x[1000];
int xx;
static void processArrays() {
	int i,k,y[30],m,n;  char s[32]; float yy[30];
	for (i=0,xx=0,m=0;i<arraycnt;i++)
		if (arrayg[i]) {
			if ((arrayt[i]==C_INTEGER)||(arrayt[i]==C_BOOL))
				k=sscanf(arrays[i],
				"N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d",
				&y[0],&y[1],&y[2],&y[3],&y[4],&y[5],&y[6],&y[7],&y[8],&y[9],
				&y[10],&y[11],&y[12],&y[13],&y[14],&y[15],&y[16],&y[17],&y[18],&y[19],
				&y[20],&y[21],&y[22],&y[23],&y[24],&y[25],&y[26],&y[27],&y[28],&y[29]
				);
			else {
				k=sscanf(arrays[i],
				"F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g",
				&yy[0],&yy[1],&yy[2],&yy[3],&yy[4],&yy[5],&yy[6],&yy[7],&yy[8],&yy[9],
				&yy[10],&yy[11],&yy[12],&yy[13],&yy[14],&yy[15],&yy[16],&yy[17],&yy[18],&yy[19],
				&yy[20],&yy[21],&yy[22],&yy[23],&yy[24],&yy[25],&yy[26],&yy[27],&yy[28],&yy[29]
				);
				for (n=0; n<k; n++)
					y[n]=(int)(yy[n]*10.0+0.5);
			}
			assert(k==arrayn[i]);
			m++;
			for (;--k>=0;) {
				for (n=0;n<xx;n++)
					if (y[k]==x[n]) break;
					if (n>=xx) x[xx++]=y[k];
			}
		}
	if (xx||needMask) {
		EMIT1("sub","7");
		if (needMask) { //L1=-1 L2=0:14 generate bit mask and its complement in L1,L2
			if (xx) EMIT4("chkl","2","0","0,0,33","s999");
			EMIT2("setv","L1","N1");
			EMIT("s997:");
			EMIT2("decvjnl","L2","s998");
			EMIT2("mulv","L1","N2");
			EMIT1("jmpl","s997");
			EMIT("s998:");
			EMIT2("setv","L2","N-1");
			EMIT2("subv","L2","L1");
			EMIT("rets");
			if (xx) EMIT("s999:");
		}
		for (i=0;i<arraycnt;i++)
			if (arrayg[i]) {
				sprintf(s,"s%d",700+i);
				m--;
				if (m) EMIT4("chkl","2",s+2,"3,0,33",s); //branch != false
				else EMIT1("jmpl",s);
			}
		for (i=0;i<arraycnt;i++)
			if (arrayg[i]) {
				sprintf(s,"s%d:",700+i);
				EMIT(s);
				if ((arrayt[i]==C_INTEGER)||(arrayt[i]==C_BOOL))
					k=sscanf(arrays[i],
					"N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d,N%d",
					&y[0],&y[1],&y[2],&y[3],&y[4],&y[5],&y[6],&y[7],&y[8],&y[9],
					&y[10],&y[11],&y[12],&y[13],&y[14],&y[15],&y[16],&y[17],&y[18],&y[19],
					&y[20],&y[21],&y[22],&y[23],&y[24],&y[25],&y[26],&y[27],&y[28],&y[29]
					);
				else {
					k=sscanf(arrays[i],
					"F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g,F%g",
					&yy[0],&yy[1],&yy[2],&yy[3],&yy[4],&yy[5],&yy[6],&yy[7],&yy[8],&yy[9],
					&yy[10],&yy[11],&yy[12],&yy[13],&yy[14],&yy[15],&yy[16],&yy[17],&yy[18],&yy[19],
					&yy[20],&yy[21],&yy[22],&yy[23],&yy[24],&yy[25],&yy[26],&yy[27],&yy[28],&yy[29]
					);
					for (n=0; n<k; n++)
						y[n]=(int)(yy[n]*10.0+0.5);
				}
				assert(k==arrayn[i]);
				for (m=0;m<k;m++) {
					for (n=0;n<xx;n++)
						if (y[m]==x[n]) {
							sprintf(s,"s%d",750+n);
							if (m<(k-1)) EMIT2("decvjnl","34",s);
							else EMIT1("jmpl",s);
							break;
						}
					assert(n<xx);
				}
			}
		for (i=0;i<xx;i++) {
			sprintf(s,"s%d:",750+i);
			EMIT(s);
			s[0]='N';
			EMIT3("setv","L1","2",itoa(x[i],s+1,10));
			EMIT("rets");
		}
		UNEMIT();
		EMIT("ends");
	}
}
char prog[12000];
char msgs[12000];
static int error(int code) {
	char s[38],*p,*q;
	s[0]=':';
	itoa(code,s+1,10);
	strcat(s,":");
	p=strstr(msgs,s);
	if (p==NULL) EMITline(strcat(s+1,"Unknown error"));
	else {
		q=strchr(p,'\n');
		if (q!=NULL) *q=0;
		EMITline(p+1);
	}
	return M_FAIL;
}
static int error2(int code, char ss[]) {
	char s[38],*p,*q;
	s[0]=':';
	itoa(code,s+1,10);
	strcat(s,":");
	p=strstr(msgs,s);
	if (p==NULL) EMITline(strcat(s+1,"Unknown error"));
	else {
		q=strchr(p,'\n');
		if (q!=NULL) *q=0;
		else q=strchr(p,'\0');
		sprintf(q+1,p+1,ss,ss,ss);
		EMITline(q+1);
	}
	return M_FAIL;
}
static int error3(int code, int arg) {
	char s[8];
	itoa(arg,s,10);
	return error2(code,s);
}
int main(int argc, char * argv[]) {
	int i,j,k,m,err;
	Context c;
	FILE *f;
	printf(";mc copyright bobcook47@hotmail.com mar.03/07\n");
	if (argc<2) {
		printf("parser filename [-s] [-o]\n");
		exit(-1);
	}
	f=fopen("msgs.txt","r");
	msgs[0]=0;
	if (f!=NULL) {
		i=fread(msgs,1,sizeof(msgs)-1,f);
		if (i>=0) msgs[i]=0;
		fclose(f);
	}
	f=fopen(argv[1],"r");
	if (f!=NULL) fclose(f);
	output=fopen("output.txt","w");
	if ((argc>2)&&(strcmp(argv[2],"-s")==0)) assemble=0;
	if ((argc>2)&&(strcmp(argv[2],"-o")==0)) {fclose(output); output=stdout;}
	if ((argc>3)&&(strcmp(argv[3],"-s")==0)) assemble=0;
	if ((argc>3)&&(strcmp(argv[3],"-o")==0)) {fclose(output); output=stdout;}
	//strcpy(prog,"cl /EP ");
	strcpy(prog,"mcpp ");
	strcat(prog,argv[1]);
	strcat(prog," >temp.txt");
	i=system(prog);
	if (i) {fprintf(output,"preprocessor error; check stdout for message\n"); exit(-1);}
#ifdef UNDER_CE
	f=fopen("\\temp.txt","r");
#else
	f=fopen("temp.txt","r");
#endif
	if (f==NULL) return -1;
	i=fread(prog,1,sizeof(prog),f);
	if (i<=0) {fclose(f); return -1;}
	prog[i]=0;
	fclose(f);
#ifdef UNDER_CE
	remove("\\temp.txt");
#else
	remove("temp.txt");
#endif
/* WARNING: ScriptEd treats numbers w leading zeros 09 as 0 */
	j=0;  c.lineno=0; c.task=-1;
	strcpy(c.names[0],"Watch");  strcpy(c.lasmName[0],"#R14,0");
	strcpy(c.names[1],"FirmwareVersion");  strcpy(c.lasmName[1],"#R35, 0");
	strcpy(c.names[2],"BatteryLevel");  strcpy(c.lasmName[2],"#R34, 0");
	strcpy(c.names[3],"ProgramSlot");  strcpy(c.lasmName[3],"#R8, 0");
	strcpy(c.names[4],"Message");  strcpy(c.lasmName[4],"#R15, 0");
	strcpy(c.names[5],"Sensor1Raw");  strcpy(c.lasmName[5],"#R12, 0");
	strcpy(c.names[6],"Sensor2Raw");  strcpy(c.lasmName[6],"#R12, 1");
	strcpy(c.names[7],"Sensor3Raw");  strcpy(c.lasmName[7],"#R12, 2");
	strcpy(c.names[8],"MotorAStatus");  strcpy(c.lasmName[8],"#R3, 0");
	strcpy(c.names[9],"MotorBStatus");  strcpy(c.lasmName[9],"#R3, 1");
	strcpy(c.names[10],"MotorCStatus");  strcpy(c.lasmName[10],"#R3, 2");
	strcpy(c.names[11],"Timer10ms3");  strcpy(c.lasmName[11],"#R26, 3");
	strcpy(c.names[12],"Sensor1");  strcpy(c.lasmName[12],"#R9, 0");
	strcpy(c.names[13],"Sensor2");  strcpy(c.lasmName[13],"#R9, 1");
	strcpy(c.names[14],"Sensor3");  strcpy(c.lasmName[14],"#R9, 2");
	strcpy(c.names[15],"Sensor1Mode");  strcpy(c.lasmName[15],"#R11, 0");
	strcpy(c.names[16],"Sensor2Mode");  strcpy(c.lasmName[16],"#R11, 1");
	strcpy(c.names[17],"Sensor3Mode");  strcpy(c.lasmName[17],"#R11, 2");
	strcpy(c.names[18],"Sensor1Type");  strcpy(c.lasmName[18],"#R10, 0");
	strcpy(c.names[19],"Sensor2Type");  strcpy(c.lasmName[19],"#R10, 1");
	strcpy(c.names[20],"Sensor3Type");  strcpy(c.lasmName[20],"#R10, 2");
	strcpy(c.names[21],"Sensor1Bool");  strcpy(c.lasmName[21],"#R13, 0");
	strcpy(c.names[22],"Sensor2Bool");  strcpy(c.lasmName[22],"#R13, 1");
	strcpy(c.names[23],"Sensor3Bool");  strcpy(c.lasmName[23],"#R13, 2");
	strcpy(c.names[24],"TaskEvent");   strcpy(c.lasmName[24],"#R23, 10");
	strcpy(c.names[25],"TaskEvent0");  strcpy(c.lasmName[25],"#R23, 0");
	strcpy(c.names[26],"TaskEvent1");  strcpy(c.lasmName[26],"#R23, 1");
	strcpy(c.names[27],"TaskEvent2");  strcpy(c.lasmName[27],"#R23, 2");
	strcpy(c.names[28],"TaskEvent3");  strcpy(c.lasmName[28],"#R23, 3");
	strcpy(c.names[29],"TaskEvent4");  strcpy(c.lasmName[29],"#R23, 4");
	strcpy(c.names[30],"TaskEvent5");  strcpy(c.lasmName[30],"#R23, 5");
	strcpy(c.names[31],"TaskEvent6");  strcpy(c.lasmName[31],"#R23, 6");
	strcpy(c.names[32],"TaskEvent7");  strcpy(c.lasmName[32],"#R23, 7");
	strcpy(c.names[33],"TaskEvent8");  strcpy(c.lasmName[33],"#R23, 8");
	strcpy(c.names[34],"TaskEvent9");  strcpy(c.lasmName[34],"#R23, 9");
	strcpy(c.names[35],"Timer0");  strcpy(c.lasmName[35],"#R1, 0");
	strcpy(c.names[36],"Timer1");  strcpy(c.lasmName[36],"#R1, 1");
	strcpy(c.names[37],"Timer2");  strcpy(c.lasmName[37],"#R1, 2");
	strcpy(c.names[38],"Timer3");  strcpy(c.lasmName[38],"#R1, 3");
	strcpy(c.names[39],"Timer10ms0");  strcpy(c.lasmName[39],"#R26, 0");
	strcpy(c.names[40],"Timer10ms1");  strcpy(c.lasmName[40],"#R26, 1");
	strcpy(c.names[41],"Timer10ms2");  strcpy(c.lasmName[41],"#R26, 2");
	strcpy(c.names[42],"Timer10ms3");  strcpy(c.lasmName[42],"#R26, 3");
	c.nNames=42;
	err=state(prog,&j,i,&c);
	if ((maxTask+maxLocal)>16) {
		fprintf(output,"ERROR |locals|>16 subroutines=%d tasks=%d\n",maxLocal,maxTask);
		exit(-1);
	}
	if (err!=M_NOERR) {
		i=j-linestart;
		if (i>0) {char *p; char s[120],t[120];
			p=strchr(prog+linestart,'\n');
			if (p) *p=0; else prog[j]=0;
			strcpy(s,code.code[code.count-1].line);
			UNEMIT();
			EMITline(prog+linestart);
			memset(t,'^',i);  t[i]=0;
			EMITline(t);
			EMITline(s);
		}
		EMITline("<--ERROR");
	} else {
		processArrays();
	}
	processJumps();
	i=code.count; j=0;
	for (;--i>=0;j++) {char arg1[16],arg2[16],arg3[16],arg4[16],arg5[16],arg6[16];
		if (code.code[j].nArgs==-1) {
			fprintf(output,"%s\n",code.code[j].line); continue;
		}
		//if (code.code[j].opcode[0]=='C') code.code[j].opcode[0]=' ';
		if (assemble) optimize(j);
		k=code.code[j].nArgs;  m=0;
		strcpy(arg1,code.code[j].arg1);
		strcpy(arg2,code.code[j].arg2);
		strcpy(arg3,code.code[j].arg3);
		strcpy(arg4,code.code[j].arg4);
		strcpy(arg5,code.code[j].arg5);
		strcpy(arg6,code.code[j].arg6);
		if (assemble) {
		  if (code.code[j].opcode[0]!=';') {
			if (strncmp(code.code[j].opcode,"chk",3)==0) {
				C1(arg1); C(arg2); C1(arg3); C(arg4);
			} else if (strcmp(code.code[j].opcode,"wait")==0) {
				C1(arg1);
			} else if (strcmp(code.code[j].opcode,"view")==0) {
				C1(arg1);
			} else if (strcmp(code.code[j].opcode,"monel")==0) {
				C1(arg1);  C(arg2);
			} else if (strcmp(code.code[j].opcode,"monal")==0) {
				C1(arg1);  C(arg2);
			} else if (strcmp(code.code[j].opcode,"event")==0) {
				C1(arg1);
			} else if (strcmp(code.code[j].opcode,"msg")==0) {
				C1(arg1);
			} else if (strcmp(code.code[j].opcode,"log")==0) {
				C1(arg1);
			} else if (strcmp(code.code[j].opcode,"set")==0) {
				if (strcmp(arg2,"36")==0) {
					C1(arg1); C(arg3);
				} else {
					C(arg1); C1(arg3);
				}
				C(arg2);  C1(arg4);
			} else if (strcmp(code.code[j].opcode,"addav")==0) {
				strcpy(code.code[j].opcode,"sumv");
				C(arg1); C(arg2); C(arg3); C(arg4);
			} else if (strcmp(code.code[j].opcode,"lea")==0) {
				strcpy(code.code[j].opcode,"setv");
				C(arg1); C(arg2); strcpy(arg3,arg2); strcpy(arg2,"2"); k=3;
			} else {
				C(arg1); C1(arg2); C(arg3); C(arg4); C(arg5); C(arg6); 
			}
		  }
		}
		fprintf(output,"\t%s\t",code.code[j].opcode);
		if (k) {fprintf(output,"%c%s",(m)?',':' ',arg1);k--,m++;}
		if (k) {fprintf(output,"%c%s",(m)?',':' ',arg2);k--,m++;}
		if (k) {fprintf(output,"%c%s",(m)?',':' ',arg3);k--,m++;}
		if (k) {fprintf(output,"%c%s",(m)?',':' ',arg4);k--,m++;}
		if (k) {fprintf(output,"%c%s",(m)?',':' ',arg5);k--,m++;}
		if (k) {fprintf(output,"%c%s",(m)?',':' ',arg6);k--,m++;}
		fprintf(output,"\n");
		S(code.code[j].arg1); S(code.code[j].arg2); S(code.code[j].arg3); S(code.code[j].arg4); S(code.code[j].arg5); S(code.code[j].arg6);

	}
	return err?-1:0;
}
