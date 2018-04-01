#include <windows.h>                         
#include <time.h>
#include <assert.h>
#include <conio.h>
#include <ctype.h>
#include <math.h>
#include <math.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//*****************************************************************************
#define FORLOOP               0
#define WHILELOOP             1
#define GOSUBLOOP             2
#define MAXDIMS               4
#define TOK_LENGTH           32
#define VAR_NAME_LEN         32
#define MAXLINENUMBER   1048756	
#define MAXSTRINGVAR       2048   // Each BASIC string VARIABLE can hold 2k

//*****************************************************************************
//                Simplified the P2C  Try-Catch macros below
//*****************************************************************************

typedef struct BASIC_jmp_buf
{
   struct BASIC_jmp_buf *next;
   jmp_buf jbuf;                 // jmp_buf is a type defined in setjmp.h
}  BASIC_jmp_buf;
//*****************************************************************************
#define TRY(x) do {BASIC_jmp_buf __try_jb;__try_jb.next=Top_of_Jump_Buffer;if(!setjmp((Top_of_Jump_Buffer = &__try_jb)->jbuf)){
#define CATCH(x,L) Top_of_Jump_Buffer = __try_jb.next; } else {if(0){L: Top_of_Jump_Buffer = __try_jb.next;}
#define END_TRY(x)  } } while (0)

//*****************************************************************************
//            I renamed what few P2C Library Prototypes that we need
//*****************************************************************************

int             Esc_Code;
int             io_Result;
int             Critical_Escape  (int);
char*           Escape_Message   (char *,int,int,char*);
BASIC_jmp_buf*  Top_of_Jump_Buffer;  

//***************************************************************************** 
//                           Enumerated the token list
//*****************************************************************************
//  IMPORTANT:  DO NOT REARRANGE THE (ORIGINAL) TOP-HALF OF THIS ENUM 
//  Dave Gillespie has some tests (< than this token > than that token) that
//  will will break some parsing ( Particularly, FOR-NEXT )
//*****************************************************************************
typedef enum {
    tokvar, toknum, tokstr, tokSyntaxError, tokplus, tokminus, toktimes, tokdiv,
    tokup, toklp, tokrp, tokcomma, toksemi, tokcolon, tokeq, toklt, tokgt,
    tokle, tokge, tokne, tokand, tokor, tokxor, tokmod, toknot, toksqr,
    toksqrt, toksin, tokcos, toktan, tokatan, toklog, toklog10, tokexp, tokabs,
    toksgn, tokhex_, tokstr_, tokval, tokchr_, tokasc, toklen, tokmid_, 
    tokrem, toklet, tokprint, tokinput, tokgoto, tokif, tokend, tokstop,
    tokfor, toknext, tokwhile, tokwend, tokgosub, tokreturn, tokread, tokdata,
    tokrestore, toksave, tokon, tokdim, toklist, tokrun, toknew, tokload, 
	tokmerge, tokbye, tokdel, tokrenum, tokthen, tokelse, tokto, tokstep, 
//*****************************************************************************
//                    Additional tokens by Kevin Diggins
//*****************************************************************************
    tokucase_, toklcase_, tokmcase_, toklocate, toksetcursor, tokcolor, tokleft_,
    tokright_, tokextract_, tokremain_, tokremove_, toktime_, tokdate_,tokcls,
    tokireplace_, toklike, toktrim_, tokltrim_, tokrtrim_, tokmsgbox, tokdo,
    tokloop, tokuntil, tokspace_, tokusing_, tokshell, tokopen, tokas, tokoutput,
    tokfprint, tokline, tokflineinput, tokclose, tokeof, tokrewind, toktrue, 
    tokfalse, tokrnd, tokrandomize, tokkill, tokcurdir_, toksysdir_, toktempdir_, 
	tokpi, tokwindir_, tokexist, tokenc_, tokfindfirst_, tokfindnext_, tokappend, 
	tokbinary, tokget,tokput,tokseek, tokinputbox,toklof,toksleep,tokinstr,
	tokrepeat_, toktextmode, tokverify, tokinkey_, tokkeypress, toktimer, 
	tokround, tokswap, tokint, tokclear, tokcrlf_,tokokcancel,tokyncancel
} tokenkinds;
//*****************************************************************************

typedef struct tokenrec {
	struct tokenrec *next;
	long kind;
	struct varrec *vp;
	double num;
	char *sp;
	char BadSyntaxChar;
} tokenrec;

//*****************************************************************************

typedef struct varrec {
    char name [VAR_NAME_LEN + 1];
    struct varrec *next;
    long dims[MAXDIMS];
    int numdims;
    BOOL IsStringVar;
    double *arr;
    double *AddressOfDoubleVar, DoubleVar;  // originally named: *val, rv;
    char **sarr;
    char **AddressOfStringVar, *StringVar;  // originally named: **sval, *sv;
} varrec;

//*****************************************************************************

typedef struct linerec {
    long num, num2;
    tokenrec *txt;
    struct linerec *next;
} linerec;

//*****************************************************************************

typedef struct valrec {
    BOOL IsStringVal;
    double val;
    char *sval;
} valrec;

//*****************************************************************************

typedef struct looprec {
    struct looprec *next;
    linerec *homeline;
    tokenrec *hometok;
    long kind;
    varrec *vp;
    double max, step;
} looprec;

//*****************************************************************************

struct LOC_exec {
    BOOL gotoflag;
	BOOL elseflag;
    tokenrec *Token;
};

//*****************************************************************************
//                            CHIPMUNK Prototypes
//*****************************************************************************

valrec factor     (struct LOC_exec*);
valrec Expression (struct LOC_exec*);

void Execute(void);
void MismatchError(void);
void ClearLoops(void);
void SyntaxError(void);
void BadSubScript(void);
void RestoreData(void);
void ClearVars(void);
void DisposeTokens(tokenrec**);
void Parse(char*,tokenrec**);
void ParseInput(tokenrec**);
void ListTokens(FILE*, tokenrec*);
void cmdload(BOOL,char*,struct LOC_exec*);
void cmdbye(void);
void ClearAll(void);
void BumpUp(void);
void BumpDown(void);
void Initialize(void);

int  IsNumber(char*);


//*****************************************************************************
//                Support and Prototypes (from the BCX project)
//*****************************************************************************

typedef struct _FILEHANDLETYPE {
    int FileNumber;
    FILE *FilePtr;
} FILEHANDLETYPE, *LPFILEHANDLETYPE;

//*****************************************************************************
void Setup_Console(void);
void Cursorsh  (int);
void TextMode (int);
void DoEvents (void);
BOOL Exist(char *);
BOOL Exist_A(char *);
BOOL Exist_B(char *);
int  Verify (char *, char *);
int  VerifyInstr(char*,char*,int=0);
char *enc (char*);
char *join (int, ... );
char *mid (char*, int, int=-1);
char *str (double);
char *space (int);
char *Using(char*, double);
char *trim(char*);
char *ltrim(char*);
char *rtrim (char*,char=32);
char *ucase(char*);
char *lcase(char*);
char *mcase(char*);
char *left(char*, int);
char *right(char*, int);
char *extract(char*, char*);
char *remain(char*, char*);
char *RemoveStr(char*, char*);
char *iReplace(char*, char*, char*);
char *timef(void);
char *findfirst(char*);
char *findnext(void);
char *repeat (char*,int);
char *windir(void);
char *curdir(void);
char *sysdir(void);
char *tempdir(void);
char *_stristr_(char*, char*);
char *inkey(void);
double Round (double, int);

int instr(char*, char*, int = 0, int = 0);
int like(char*, char*);
int EoF(FILE*);
int keypress (void);

void Center(HWND);
void cls(void);
void color(long, long);
void locate(int, int, int = 1, int = 12);
void swap(char*, char*, int);
char *crlf(void);

HWND GetConsoleWndHandle(void);

void OK_CANCEL (char*,char*);
int  YN_CANCEL (char*,char*);

//*****************************************************************************
// Controls auto-indenting loops when LIST-ed - called by BumpUp() & BumpDown()
//*****************************************************************************

static int     Indent;
static char    Scoot[MAXSTRINGVAR];
 
//*****************************************************************************
//                        InputBox$ Prototypes
//*****************************************************************************

char    BCX_INPUTBOX_VAL[MAXSTRINGVAR] = {0};
LPWORD  lpwAlign(LPWORD);
char*   InputBox(char*,char*,char*);
LRESULT CreatePrompter(char*,char*,char*);
LRESULT CALLBACK Prompter(HWND,UINT,WPARAM,LPARAM);

//*****************************************************************************
//                    FILE Prototypes by Kevin Diggins
//*****************************************************************************

#define GET(A,B,C)fread(B,1,C,A)
#define PUT(A,B,C)fwrite(B,1,C,A)

DWORD lof (char*);
FILE* GetFilePtr(int);
void  SetFilePtr(int, FILE *);
void  UnSetFilePtr(int);
void  OpenFileForReading(int, char*);
void  OpenFileForWriting(int, char*);
void  OpenFileForAppending(int, char*);
void  OpenFileForBinary(int, char*);

//*****************************************************************************
//                       BCX GetFileName$ used in 'LOAD'
//*****************************************************************************

char *GetFileName (char*,char*,int=0,HWND=0,DWORD=0,char* =0,char* =0,int* =0);
LRESULT CALLBACK SBProc (int, WPARAM, LPARAM);
HHOOK CmDlgHook;
char* OurLoadedFname; 
//*****************************************************************************
//                           Chipmunk Globals
//*****************************************************************************

static char *inbuf;
static linerec *linebase;
static varrec *varbase;
static looprec *loopbase;
static long curline;
static linerec *stmtline, *dataline;
static tokenrec *stmttok, *datatok, *buf;
static int exitflag;


//*****************************************************************************
//                           My additional globals
//*****************************************************************************

static int  Dirtyflag = {0};
static char DateStr[255]= {0};
static HANDLE FileHandle;
static FILEHANDLETYPE FILEREC[256];
static WIN32_FIND_DATA FindData;

//*****************************************************************************
//                       Affects our console window
//*****************************************************************************

void HiLite(FILE*,char*);
COORD cursor;
HWND hConsole;
int color_fg = 7;
int color_bg = 0;


