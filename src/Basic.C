//*****************************************************************************
#define VERSION "Gillespie BASIC v1.58 for Windows by Kevin Diggins (2026)\n"
//              Based on Chipmunk Basic 1.0 by Dave Gillespie
//       1.58 includes memory safety improvements by Google Gemini
//       1.58 replaced linear token lookup with fast hash lookup by Claude
//       1.58 Supports simple user-defined FUNCTIONS with up to 10 arguments
//            User Functions must be read before they can be reliably used.
//       1.58 Improved with BCX's enhanced USING$() function
//*****************************************************************************
#define FGCOLOR 10  //  Window Foreground Color   (14)
#define BGCOLOR  0  //  Window Background Color   (01)
#define KWCOLOR  9  //  Keyword highlight color

#ifdef _MSC_VER
#pragma warning(disable: 4611)  // setjmp/longjmp interaction warning - safe in C
#pragma warning(disable: 4324)  // setjmp/longjmp interaction warning - safe in C 
#pragma warning(disable : 4996) // deprecated GetVersionExA
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "Basic.h"

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"comdlg32.lib")



void Initialize(void)
{
	InitializeKeywordHashTable();
	G_inbuf = (char*)safe_calloc(MAXSTRINGVAR, 1);
	OurLoadedFname = (char*)calloc(1024, 1);
	linebase = NULL;
	varbase = NULL;
	loopbase = NULL;
	exitflag = FALSE;
	Dirtyflag = FALSE;
	funcbase = NULL;
	ClearAll();
	Setup_Console();
	printf(VERSION);
}


char* mystrDate(char* buffer)
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	// Ensure we don't overflow the 11-byte expected buffer
	strftime(buffer, 11, "%m/%d/%Y", timeinfo);
	return buffer;
}



int main(int argc, char* argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
	Top_of_Jump_Buffer = NULL;
	//******************************
	Initialize();
	//******************************
	do
	{
		if (!Dirtyflag++)
		{
			//			strcpy(G_inbuf, "load");     // uncomment if you want the OpenFile to always display on Startup
		}
		else
		{
			printf("\n%s", "Ready: ");
			GB_gets(G_inbuf, MAXSTRINGVAR);
		}
		//********************************************************************
		//    This allows us to invoke the Windows GetFileName dialogbox
		//********************************************************************
		if (_stricmp(G_inbuf, "load") == 0)
			sprintf(G_inbuf, "%s%s", "load ", enc(""));
		//********************************************************************
		ParseInput(&tr_buf);	// buf is a tokenrec ptr

		if (curline == 0)	// if true, then we typed an IMMEDIATE mode command
		{
			stmtline = NULL;	// stmtline is a global linerec ptr
			stmttok = tr_buf;	// stmttok is a global tokenrec ptr
			if (stmttok != NULL)
				Execute();	// execute IMMEDIATE mode command
			DisposeTokens(&tr_buf);	//free some memory
		}
	} while (!(exitflag));
	free(G_inbuf);
	return 0;
	//****************************************************************************
}

void ParseInput(tokenrec** buf)
{
	linerec* L1, * L2, * L3;
	curline = 0;

	// Safety check for trim output
	char* trimmed = trim(G_inbuf);
	if (trimmed != G_inbuf)
	{
		strncpy(G_inbuf, trimmed, MAXSTRINGVAR - 1);
		G_inbuf[MAXSTRINGVAR - 1] = '\0';
	}

	while (G_inbuf[0] != '\0' && isdigit((unsigned char)G_inbuf[0]))
	{
		curline = curline * 10 + (G_inbuf[0] - '0');
		// FIX: Replaced strcpy with memmove for overlapping memory
		memmove(G_inbuf, G_inbuf + 1, strlen(G_inbuf));
	}

	Parse(G_inbuf, buf);

	if (curline == 0)
		return;

	L1 = linebase;
	L2 = NULL;

	while (L1 != NULL && L1->num < curline)
	{
		L2 = L1;
		L1 = L1->next;
	}

	if (L1 != NULL && L1->num == curline)
	{
		L3 = L1;
		L1 = L1->next;
		if (L2 == NULL)
			linebase = L1;
		else
			L2->next = L1;
		DisposeTokens(&L3->txt);
		free(L3);
	}

	if (*buf != NULL)
	{
		L3 = (linerec*)safe_calloc(1, sizeof(linerec));
		L3->next = L1;
		if (L2 == NULL)
			linebase = L3;
		else
			L2->next = L3;
		L3->num = curline;
		L3->txt = *buf;
	}
	ClearLoops();
	RestoreData();
}

//******************************************
//      Hash table for keyword lookup
//******************************************

#define HASH_TABLE_SIZE 128

typedef struct KeywordEntry
{
	char* keyword;
	int token_kind;
	struct KeywordEntry* next;
} KeywordEntry;

static KeywordEntry* keyword_hash_table[HASH_TABLE_SIZE] = { 0 };

// Simple hash function for keywords
static unsigned int hash_keyword(const char* str)
{
	unsigned int hash = 0;
	while (*str)
	{
		hash = (hash * 31 + tolower((unsigned char)*str)) % HASH_TABLE_SIZE;
		str++;
	}
	return hash;
}

// Initialize keyword hash table (call once at startup)
void InitializeKeywordHashTable(void)
{
	static int initialized = 0;
	if (initialized) return;
	initialized = 1;

	// Define keyword-token pairs
	struct
	{
		char* keyword;
		int token;
	} keywords[] =
	{
	  {"and", tokand}, {"or", tokor}, {"xor", tokxor}, {"not", toknot},
	  {"mod", tokmod}, {"sqr", toksqr}, {"sqrt", toksqrt}, {"round", tokround},
	  {"sin", toksin}, {"cos", tokcos}, {"tan", toktan}, {"atn", tokatan},
	  {"log10", toklog10}, {"log", toklog}, {"exp", tokexp}, {"abs", tokabs},
	  {"int", tokint}, {"sgn", toksgn}, {"keypress", tokkeypress}, {"timer", toktimer},
	  {"rnd", tokrnd}, {"str$", tokstr_}, {"val", tokval}, {"hex$", tokhex_},
	  {"lof", toklof}, {"like", toklike}, {"verify", tokverify}, {"exist", tokexist},
	  {"sleep", toksleep}, {"textmode", toktextmode}, {"randomize", tokrandomize},
	  {"setcursor", toksetcursor}, {"chr$", tokchr_}, {"time$", toktime_},
	  {"date$", tokdate_}, {"inkey$", tokinkey_}, {"curdir$", tokcurdir_},
	  {"windir$", tokwindir_}, {"sysdir$", toksysdir_}, {"tempdir$", toktempdir_},
	  {"trim$", toktrim_}, {"environ$", tokenviron_}, {"ltrim$", tokltrim_},
	  {"rtrim$", tokrtrim_}, {"ucase$", tokucase_}, {"enc$", tokenc_},
	  {"enclose$", tokenc_}, {"findfirst$", tokfindfirst_}, {"findnext$", tokfindnext_},
	  {"lcase$", toklcase_}, {"mcase$", tokmcase_}, {"space$", tokspace_},
	  {"left$", tokleft_}, {"right$", tokright_}, {"repeat$", tokrepeat_},
	  {"extract$", tokextract_}, {"instr", tokinstr}, {"remain$", tokremain_},
	  {"retain$", tokretain_}, {"remove$", tokremove_}, {"ireplace$", tokireplace_},
	  {"inputbox$", tokinputbox}, {"ok_cancel", tokokcancel}, {"yn_cancel", tokyncancel},
	  {"using$", tokusing_}, {"asc", tokasc}, {"len", toklen}, {"mid$", tokmid_},
	  {"let", toklet}, {"print", tokprint}, {"input", tokinput}, {"output", tokoutput},
	  {"fprint", tokfprint}, {"line", tokflineinput}, {"get", tokget}, {"put", tokput},
	  {"seek", tokseek}, {"true", toktrue}, {"false", tokfalse}, {"pi", tokpi},
	  {"crlf$", tokcrlf_}, {"eof", tokeof}, {"close", tokclose}, {"rewind", tokrewind},
	  {"goto", tokgoto}, {"if", tokif}, {"end", tokend}, {"stop", tokstop},
	  {"for", tokfor}, {"next", toknext}, {"while", tokwhile}, {"wend", tokwend},
	  {"do", tokdo}, {"loop", tokloop}, {"until", tokuntil}, {"gosub", tokgosub},
	  {"return", tokreturn}, {"read", tokread}, {"data", tokdata}, {"restore", tokrestore},
	  {"locate", toklocate}, {"gotoxy", tokgotoxy}, {"color", tokcolor}, {"on", tokon},
	  {"dim", tokdim}, {"list", toklist}, {"run", tokrun}, {"cls", tokcls},
	  {"msgbox", tokmsgbox}, {"new", toknew}, {"load", tokload}, {"shell", tokshell},
	  {"eval", tokeval}, {"kill", tokkill}, {"open", tokopen}, {"as", tokas},
	  {"append", tokappend}, {"binary", tokbinary}, {"merge", tokmerge},
	  {"save", toksave}, {"bye", tokbye}, {"quit", tokbye}, {"exit", tokbye},
	  {"del", tokdel}, {"renum", tokrenum}, {"then", tokthen}, {"else", tokelse},
	  {"to", tokto}, {"step", tokstep}, {"clear", tokclear}, {"swap", tokswap},
	  {"rem", tokrem},{"function", tokfunction},
	  {NULL, 0}
	};

	// Insert keywords into hash table
	for (int i = 0; keywords[i].keyword != NULL; i++)
	{
		unsigned int hash = hash_keyword(keywords[i].keyword);
		KeywordEntry* entry = (KeywordEntry*)malloc(sizeof(KeywordEntry));
		entry->keyword = keywords[i].keyword;
		entry->token_kind = keywords[i].token;
		entry->next = keyword_hash_table[hash];
		keyword_hash_table[hash] = entry;
	}
}

// Fast keyword lookup using hash table
static int LookupKeyword(const char* token)
{
	unsigned int hash = hash_keyword(token);
	KeywordEntry* entry = keyword_hash_table[hash];

	while (entry)
	{
		if (_stricmp(entry->keyword, token) == 0)
		{
			return entry->token_kind;
		}
		entry = entry->next;
	}
	return -1; // Not a keyword
}

// Optimized number parsing
static double FastParseNumber(const char* str, int* chars_consumed)
{
	double n = 0.0;
	double d = 1.0;
	double d1 = 1.0;
	int i = 0;

	// Parse integer and decimal parts
	while (str[i] && (isdigit((unsigned char)str[i]) || (str[i] == '.' && d1 == 1)))
	{
		if (str[i] == '.')
		{
			d1 = 10.0;
		}
		else
		{
			n = n * 10 + (str[i] - '0');
			d *= d1;
		}
		i++;
	}
	n /= d;

	// Parse scientific notation
	if (str[i] && (str[i] == 'E' || str[i] == 'e'))
	{
		i++;
		d1 = 10.0;
		if (str[i] && (str[i] == '-' || str[i] == '+'))
		{
			if (str[i] == '-') d1 = 0.1;
			i++;
		}
		int exp = 0;
		while (str[i] && isdigit((unsigned char)str[i]))
		{
			exp = exp * 10 + (str[i] - '0');
			i++;
		}
		for (int k = 0; k < exp; k++)
		{
			n *= d1;
		}
	}

	*chars_consumed = i;
	return n;
}

// Optimized Parse function with reduced allocations
void Parse(char* inbuf, tokenrec** buf)
{
	long i = 0;
	int len = (int)strlen(inbuf);
	char token[TOK_LENGTH + 1];
	char ch;
	tokenrec* t, * tptr;
	varrec* v;

	tptr = NULL;
	*buf = NULL;

	while (i < len)
	{
		// Skip whitespace
		while (i < len && inbuf[i] == ' ') i++;
		if (i >= len) break;

		ch = inbuf[i++];

		// Allocate token
		t = (tokenrec*)calloc(sizeof(tokenrec), 1);
		if (tptr == NULL)
			*buf = t;
		else
			tptr->next = t;
		tptr = t;
		t->next = NULL;

		// Single-character tokens
		if (ch == '"')
		{
			t->kind = tokstr;
			t->sp = (char*)calloc(MAXSTRINGVAR, 1);
			int j = 0;
			while (i < len && inbuf[i] != ch && j < MAXSTRINGVAR - 1)
			{
				t->sp[j++] = inbuf[i++];
			}
			t->sp[j] = '\0';
			if (i < len) i++; // Skip closing quote
		}
		else if (ch == 39)   // REM comment
		{
			t->kind = tokrem;
			t->sp = (char*)calloc(MAXSTRINGVAR, 1);
			strncpy(t->sp, inbuf + i, MAXSTRINGVAR - 1);
			break; // Rest of line is comment
		}
		else if (ch == '+') t->kind = tokplus;
		else if (ch == '-') t->kind = tokminus;
		else if (ch == '*') t->kind = toktimes;
		else if (ch == '/') t->kind = tokdiv;
		else if (ch == '^') t->kind = tokup;
		else if (ch == '(' || ch == '[') t->kind = toklp;
		else if (ch == ')' || ch == ']') t->kind = tokrp;
		else if (ch == ',') t->kind = tokcomma;
		else if (ch == ';') t->kind = toksemi;
		else if (ch == ':') t->kind = tokcolon;
		else if (ch == '?') t->kind = tokprint;
		else if (ch == '=') t->kind = tokeq;
		else if (ch == '<')
		{
			if (i < len && inbuf[i] == '=')
			{
				t->kind = tokle;
				i++;
			}
			else if (i < len && inbuf[i] == '>')
			{
				t->kind = tokne;
				i++;
			}
			else t->kind = toklt;
		}
		else if (ch == '>')
		{
			if (i < len && inbuf[i] == '=')
			{
				t->kind = tokge;
				i++;
			}
			else t->kind = tokgt;
		}
		else if (isalpha(ch))
		{
			// Parse identifier/keyword
			i--;
			int j = 0;
			while (i < len && (inbuf[i] == '$' || inbuf[i] == '_' || isalnum(inbuf[i])) && j < TOK_LENGTH)
			{
				token[j++] = inbuf[i++];
			}
			token[j] = '\0';

			// Try keyword lookup first
			int keyword_token = LookupKeyword(token);
			if (keyword_token != -1)
			{
				t->kind = keyword_token;
				// Handle REM specially
				if (keyword_token == tokrem)
				{
					t->sp = (char*)calloc(MAXSTRINGVAR, 1);
					strncpy(t->sp, inbuf + i, MAXSTRINGVAR - 1);
					break;
				}
			}
			else
			{
				// It's a variable
				t->kind = tokvar;
				v = varbase;
				while (v != NULL && _stricmp(v->name, token))
					v = v->next;
				if (v == NULL)
				{
					v = (varrec*)calloc(sizeof(varrec), 1);
					v->next = varbase;
					varbase = v;
					strcpy(v->name, token);
					v->numdims = 0;
					if (token[j - 1] == '$')
					{
						v->IsStringVar = TRUE;
						v->StringVar = NULL;
						v->AddressOfStringVar = &v->StringVar;
					}
					else
					{
						v->IsStringVar = FALSE;
						v->DoubleVar = 0.0;
						v->AddressOfDoubleVar = &v->DoubleVar;
					}
				}
				t->vp = v;
			}
		}
		else if (isdigit(ch) || ch == '.')
		{
			// Parse number using optimized function
			i--;
			int chars_consumed;
			t->kind = toknum;
			t->num = FastParseNumber(inbuf + i, &chars_consumed);
			i += chars_consumed;
		}
		else
		{
			t->kind = tokSyntaxError;
			t->BadSyntaxChar = ch;
		}
	}
}



//*****************************************************************************

void ClearAll(void)	// tied to the new BASIC command:  CLEAR
{
	ClearVars();
	ClearLoops();
	RestoreData();
}

//*****************************************************************************

void RestoreData(void)
{
	dataline = NULL;
	datatok = NULL;
}

//*****************************************************************************

void ClearLoops(void)
{
	looprec* l;
	while (loopbase != NULL)
	{
		l = loopbase->next;
		free(loopbase);
		loopbase = l;
	}
}
//*****************************************************************************

void FreeStringArrays(struct varrec* ThisVarbase)
{
	int i, k;
	if (ThisVarbase->numdims > 0)
	{
		k = 1;
		for (i = 0; i < ThisVarbase->numdims; i++)
		{
			k = k * (ThisVarbase->dims[i]);
		}
		for (i = 0; i < k; i++)
		{
			free(ThisVarbase->sarr[i]);
		}
		free(ThisVarbase->sarr);
	}
}

//*****************************************************************************

void ClearVars(void)
{
	varrec* v;
	v = varbase;

	while (v != NULL)
	{
		if (v->numdims != 0 && v->IsStringVar == FALSE)
			free(v->arr);

		if (v->numdims != 0 && v->IsStringVar == TRUE)
			FreeStringArrays(v);

		if (v->StringVar != NULL && v->IsStringVar == TRUE)
			free(v->StringVar);

		v->numdims = 0;

		if (v->IsStringVar)
		{
			v->StringVar = NULL;
			v->AddressOfStringVar = &v->StringVar;
		}
		else
		{
			v->DoubleVar = 0.0;
			v->AddressOfDoubleVar = &v->DoubleVar;
		}
		v = v->next;
	}
}

//*****************************************************************************

char* NumToStr(char* Result, double n)
{
	char s[256] = { 0 };
	if (n != 0 && (fabs(n) < 1e-2 || fabs(n) >= 1e12))
	{
		snprintf(s, sizeof(s), "% .5E", n);
		strncpy(Result, s, MAXSTRINGVAR - 1);
		return Result;
	}
	else
	{
		snprintf(s, sizeof(s), "%30.15f", n);
		char* p = s + (int)strlen(s) - 1;
		while (p > s && *p == '0') *p-- = '\0';
		if (*p == '.') *p = '\0';
		strncpy(Result, ltrim(s), MAXSTRINGVAR - 1);
		return Result;
	}
}

//*****************************************************************************

void ListTokens(FILE* f, tokenrec* buf)
{
	char Str1[2048] = { 0 };

	switch (buf->kind)
	{
	case toknext:
	case tokloop:
	case tokwend:
		BumpDown();
	}

	fprintf(f, "%s", Scoot);

	while (buf != NULL)
	{
		switch (buf->kind)
		{
		case toklp:
			putc('(', f);
			break;

		case tokrp:
			putc(')', f);
			break;

		case tokcomma:
			putc(',', f);
			break;

		case tokfunction:
			HiLite(f, "FUNCTION ");
			break;


		case toksemi:
			putc(';', f);
			break;

		case tokvar:
			fputs(mcase(buf->vp->name), f);	//Use MCASE$ to list our variables
			break;

		case toknum:                        // numbers
			color(15, BGCOLOR);
			fputs(NumToStr(Str1, buf->num), f);
			color(FGCOLOR, BGCOLOR);
			break;

		case tokstr:                        // strings
			color(6, BGCOLOR);
			fprintf(f, "\"%s\"", buf->sp);
			color(FGCOLOR, BGCOLOR);
			break;

		case tokplus:
			fprintf(f, "+");
			break;

		case tokminus:
			fprintf(f, "-");
			break;

		case toktimes:
			fprintf(f, "*");
			break;

		case tokdiv:
			fprintf(f, "/");
			break;

		case tokup:
			fprintf(f, "^");
			break;

		case tokcolon:
			fprintf(f, " : ");
			break;

		case tokeq:
			fprintf(f, " = ");
			break;

		case toklt:
			fprintf(f, " < ");
			break;

		case tokgt:
			fprintf(f, " > ");
			break;

		case tokle:
			fprintf(f, " <= ");
			break;

		case tokge:
			fprintf(f, " >= ");
			break;

		case tokne:
			fprintf(f, " <> ");
			break;

		case tokand:
			HiLite(f, " AND ");
			break;

		case tokor:
			HiLite(f, " OR ");
			break;

		case tokxor:
			HiLite(f, " XOR ");
			break;

		case tokround:
			HiLite(f, "ROUND");
			break;

		case tokmod:
			HiLite(f, " MOD ");
			break;

		case toknot:
			HiLite(f, " NOT ");
			break;

		case toksqr:
			HiLite(f, "SQR");
			break;

		case toksqrt:
			HiLite(f, "SQRT");
			break;

		case toksin:
			HiLite(f, "SIN");
			break;

		case tokcos:
			HiLite(f, "COS");
			break;

		case toktan:
			HiLite(f, "TAN");
			break;

		case tokatan:
			HiLite(f, "ATN");
			break;

		case toklog10:
			HiLite(f, "LOG10");
			break;

		case toklog:
			HiLite(f, "LOG");
			break;

		case tokexp:
			HiLite(f, "EXP");
			break;

		case tokabs:
			HiLite(f, "ABS");
			break;

		case toksgn:
			HiLite(f, "SGN");
			break;

		case tokint:
			HiLite(f, "INT");
			break;

		case tokkeypress:
			HiLite(f, "KEYPRESS");
			break;

		case toktimer:
			HiLite(f, "TIMER");
			break;

		case tokrnd:
			HiLite(f, "RND");
			break;

		case tokstr_:
			HiLite(f, "STR$");
			break;

		case tokval:
			HiLite(f, "VAL");
			break;

		case tokhex_:
			HiLite(f, "HEX$");
			break;

		case toklof:
			HiLite(f, "LOF");
			break;

		case toklike:
			HiLite(f, "LIKE");
			break;

		case tokverify:
			HiLite(f, "VERIFY");
			break;

		case tokexist:
			HiLite(f, "EXIST");
			break;

		case toksleep:
			HiLite(f, "SLEEP");
			break;

		case toktextmode:
			HiLite(f, "TEXTMODE ");
			break;

		case tokrandomize:
			HiLite(f, "RANDOMIZE ");
			break;

		case toksetcursor:
			HiLite(f, "SETCURSOR ");
			break;

		case tokchr_:
			HiLite(f, "CHR$");
			break;

		case toktime_:
			HiLite(f, "TIME$");
			break;

		case tokinkey_:
			HiLite(f, "INKEY$");
			break;

		case tokdate_:
			HiLite(f, "DATE$");
			break;

		case tokcurdir_:
			HiLite(f, "CURDIR$");
			break;

		case tokwindir_:
			HiLite(f, "WINDIR$");
			break;

		case toksysdir_:
			HiLite(f, "SYSDIR$");
			break;

		case toktempdir_:
			HiLite(f, "TEMPDIR$");
			break;

		case toktrim_:
			HiLite(f, "TRIM$");
			break;

		case tokenviron_:
			HiLite(f, "ENVIRON$");
			break;

		case tokltrim_:
			HiLite(f, "LTRIM$");
			break;

		case tokrtrim_:
			HiLite(f, "RTRIM$");
			break;

		case tokenc_:
			HiLite(f, "ENCLOSE$");
			break;

		case tokucase_:
			HiLite(f, "UCASE$");
			break;

		case tokfindfirst_:
			HiLite(f, "FINDFIRST$");
			break;

		case tokfindnext_:
			HiLite(f, "FINDNEXT$");
			break;

		case toklcase_:
			HiLite(f, "LCASE$");
			break;

		case tokmcase_:
			HiLite(f, "MCASE$");
			break;

		case tokspace_:
			HiLite(f, "SPACE$");
			break;

		case tokleft_:
			HiLite(f, "LEFT$");
			break;

		case tokright_:
			HiLite(f, "RIGHT$");
			break;

		case tokrepeat_:
			HiLite(f, "REPEAT$");
			break;

		case tokusing_:
			HiLite(f, "USING$");
			break;

		case tokextract_:
			HiLite(f, "EXTRACT$");
			break;

		case tokinstr:
			HiLite(f, "INSTR");
			break;

		case tokremain_:
			HiLite(f, "REMAIN$");
			break;

		case tokretain_:
			HiLite(f, "RETAIN$");
			break;

		case tokireplace_:
			HiLite(f, "IREPLACE$");
			break;

		case tokinputbox:
			HiLite(f, "INPUTBOX$");
			break;

		case tokokcancel:
			HiLite(f, "OK_CANCEL ");
			break;

		case tokyncancel:
			HiLite(f, "YN_CANCEL");
			break;

		case tokremove_:
			HiLite(f, "REMOVE$");
			break;

		case tokasc:
			HiLite(f, "ASC");
			break;

		case tokeof:
			HiLite(f, "EOF");
			break;

		case tokpi:
			HiLite(f, "PI");
			break;

		case tokcrlf_:
			HiLite(f, "CRLF$");
			break;

		case toktrue:
			HiLite(f, "TRUE");
			break;

		case tokfalse:
			HiLite(f, "FALSE");
			break;

		case toklen:
			HiLite(f, "LEN");
			break;

		case tokmid_:
			HiLite(f, "MID$");
			break;

		case toklet:
			HiLite(f, "LET ");
			break;

		case tokprint:
			HiLite(f, "PRINT ");
			break;

		case tokfprint:
			HiLite(f, "FPRINT ");
			break;

		case tokflineinput:
			HiLite(f, "LINE ");
			break;

		case tokget:
			HiLite(f, "GET ");
			break;

		case tokput:
			HiLite(f, "PUT ");
			break;

		case tokseek:
			HiLite(f, "SEEK ");
			break;

		case tokappend:
			HiLite(f, "APPEND ");
			break;

		case tokbinary:
			HiLite(f, "BINARY ");
			break;

		case tokclose:
			HiLite(f, "CLOSE ");
			break;

		case tokrewind:
			HiLite(f, "REWIND ");
			break;

		case tokinput:
			HiLite(f, "INPUT ");
			break;

		case tokoutput:
			HiLite(f, "OUTPUT ");
			break;

		case tokgoto:
			HiLite(f, " GOTO ");
			break;

		case tokif:
			HiLite(f, "IF ");
			break;

		case tokend:
			HiLite(f, "END");
			break;

		case tokstop:
			HiLite(f, "STOP");
			break;

		case tokfor:
			HiLite(f, "FOR ");
			BumpUp();
			break;

		case toknext:
			HiLite(f, "NEXT ");
			break;

		case tokwhile:
			HiLite(f, "WHILE ");
			BumpUp();
			break;

		case tokwend:
			HiLite(f, "WEND ");
			break;

		case tokdo:
			HiLite(f, "DO ");
			BumpUp();
			break;

		case tokloop:
			HiLite(f, "LOOP ");
			break;

		case tokuntil:
			HiLite(f, "UNTIL ");
			break;

		case tokgosub:
			HiLite(f, "GOSUB ");
			break;

		case tokreturn:
			HiLite(f, "RETURN");
			break;

		case tokread:
			HiLite(f, "READ ");
			break;

		case tokdata:
			HiLite(f, "DATA ");
			break;

		case tokrestore:
			HiLite(f, "RESTORE ");
			break;

		case tokmsgbox:
			HiLite(f, "MSGBOX");
			break;

		case toklocate:
			HiLite(f, "LOCATE ");
			break;

		case tokgotoxy:
			HiLite(f, "GOTOXY ");
			break;

		case tokcolor:
			HiLite(f, "COLOR ");
			break;

		case tokon:
			HiLite(f, "ON ");
			break;

		case tokdim:
			HiLite(f, "DIM ");
			break;

		case toklist:
			HiLite(f, "LIST");
			break;

		case tokrun:
			HiLite(f, "RUN");
			break;

		case tokcls:
			HiLite(f, "CLS");
			break;

		case tokclear:
			HiLite(f, "CLEAR");
			break;

		case tokopen:
			HiLite(f, "OPEN ");
			break;

		case tokas:
			HiLite(f, "AS ");
			BumpDown();
			break;

		case toknew:
			HiLite(f, "NEW");
			break;

		case tokshell:
			HiLite(f, "SHELL ");
			break;

		case tokeval:
			HiLite(f, "EVAL ");
			break;

		case tokkill:
			HiLite(f, "KILL ");
			break;

		case tokload:
			HiLite(f, "LOAD ");
			break;

		case tokmerge:
			HiLite(f, "MERGE ");
			break;

		case toksave:
			HiLite(f, "SAVE ");
			break;

		case tokdel:
			HiLite(f, "DEL ");
			break;

		case tokbye:
			HiLite(f, "BYE ");
			break;

		case tokrenum:
			HiLite(f, "RENUM");
			break;

		case tokthen:
			HiLite(f, " THEN ");
			break;

		case tokelse:
			HiLite(f, " ELSE ");
			break;

		case tokto:
			HiLite(f, " TO ");
			break;

		case tokstep:
			HiLite(f, " STEP ");
			break;

		case tokswap:
			HiLite(f, "SWAP ");
			break;

		case tokrem:
			color(8, BGCOLOR);
			fprintf(f, " ' %s", buf->sp);	// All REM's become single quotes
			color(FGCOLOR, BGCOLOR);
			break;

		case tokSyntaxError:
			fprintf(f, ">>> %c <<<", buf->BadSyntaxChar);
			break;
		}
		buf = buf->next;
	}
}

//*****************************************************************************

void HiLite(FILE* f, char* KW)
{
	color(KWCOLOR, BGCOLOR);
	fprintf(f, "%s", KW);
	color(FGCOLOR, BGCOLOR);
}

//*****************************************************************************

void DisposeTokens(tokenrec** tok1)
{
	tokenrec* tok2;
	while (*tok1 != NULL)
	{
		tok2 = (*tok1)->next;
		if ((*tok1)->kind == tokrem || (*tok1)->kind == tokstr)
			free((*tok1)->sp);
		free(*tok1);
		*tok1 = tok2;
	}
}


//*****************************************************************************

void SyntaxError(void)
{
	Abort("Syntax error!");
}

//*****************************************************************************

void MismatchError(void)
{
	Abort("Mismatch error!");
}

//*****************************************************************************

void BadSubScript(void)
{
	Abort("Subscript error!");
}

//*****************************************************************************

double Doublefactor(struct LOC_exec* LINK)
{
	valrec n;
	n = factor(LINK);
	if (n.IsStringVal)
		MismatchError();
	return (n.val);
}

//*****************************************************************************

char* strfactor(struct LOC_exec* LINK)
{
	valrec n;
	n = factor(LINK);
	if (!n.IsStringVal)
		MismatchError();
	return (n.sval);
}

//*****************************************************************************

long intfactor(struct LOC_exec* LINK)
{
	return ((long)floor(Doublefactor(LINK) + 0.5));
}

//*****************************************************************************

double DoubleExpression(struct LOC_exec* LINK)
{
	valrec n;
	n = Expression(LINK);
	if (n.IsStringVal)
		MismatchError();
	return (n.val);
}

//*****************************************************************************
//                    grabs the next valid string expression
//*****************************************************************************

char* StringExpression(struct LOC_exec* LINK)
{
	valrec n;
	n = Expression(LINK);
	if (!n.IsStringVal)
		MismatchError();
	return (n.sval);
}

//*****************************************************************************
//             Store the next valid string expression into Result
//*****************************************************************************

char* StringExpressionEx(char* Result, struct LOC_exec* LINK)
{
	valrec n;
	n = Expression(LINK);
	if (!n.IsStringVal)
		MismatchError();
	strcpy(Result, n.sval);
	free(n.sval);
	return Result;
}

//*****************************************************************************

long IntegerExpression(struct LOC_exec* LINK)
{
	return ((long)floor(DoubleExpression(LINK) + 0.5));
}

//*****************************************************************************

void RequireToken(long k, struct LOC_exec* LINK)
{
	if (LINK->Token == NULL || LINK->Token->kind != k)
		SyntaxError();
	LINK->Token = LINK->Token->next;
}

//*****************************************************************************

void skipparen(struct LOC_exec* LINK)
{
	do
	{
		if (LINK->Token == NULL)
			SyntaxError();
		if (LINK->Token->kind == tokrp || LINK->Token->kind == tokcomma)
			goto _L1;
		if (LINK->Token->kind == toklp)
		{
			LINK->Token = LINK->Token->next;
			skipparen(LINK);
		}
		LINK->Token = LINK->Token->next;
	} while (TRUE);
_L1:
	;
}

//*****************************************************************************

varrec* findvar(struct LOC_exec* LINK)
{
	long i, j, k, FORLIM;
	varrec* v;
	tokenrec* tok;

	if (LINK->Token->kind == tokrem)
		return 0;	// allow apostrophe after NEXT

	if (LINK->Token == NULL || LINK->Token->kind != tokvar)
		SyntaxError();

	// Check if this is a function call
	if (LINK->Token->next != NULL && LINK->Token->next->kind == toklp)
	{
		funcrec* f = FindFunction(LINK->Token->vp->name);

		if (f != NULL)
		{
			// This is a function call, not a variable - this shouldn't happen in findvar
			// but we need to handle it gracefully
			SyntaxError();
		}
	}

	v = LINK->Token->vp;

	LINK->Token = LINK->Token->next;

	if (LINK->Token == NULL || LINK->Token->kind != toklp)
	{
		if (v->numdims != 0)
			BadSubScript();
		return v;
	}

	if (v->numdims == 0)
	{
		tok = LINK->Token;
		i = 0;
		j = 1;

		do
		{
			if (i >= MAXDIMS)
				BadSubScript();

			LINK->Token = LINK->Token->next;
			skipparen(LINK);
			j *= 11;
			i++;
			v->dims[i - 1] = 11;	// implicitely DIM 11 elements
		} while (LINK->Token->kind != tokrp);

		v->numdims = i;

		if (v->IsStringVar)
		{
			v->sarr = (char**)safe_calloc(j, sizeof(char*));
			for (k = 0; k < j; k++)
				v->sarr[k] = NULL;
		}
		else
		{
			v->arr = (double*)safe_calloc(j, sizeof(double));
			for (k = 0; k < j; k++)
				v->arr[k] = 0.0;
		}
		LINK->Token = tok;
	}

	k = 0;

	LINK->Token = LINK->Token->next;

	FORLIM = v->numdims;

	for (i = 1; i <= FORLIM; i++)
	{
		j = IntegerExpression(LINK);
		if ((unsigned long)j >= (unsigned long)v->dims[i - 1])
			BadSubScript();
		k = k * v->dims[i - 1] + j;
		if (i < v->numdims)
			RequireToken(tokcomma, LINK);
	}

	RequireToken(tokrp, LINK);

	if (v->IsStringVar)
		v->AddressOfStringVar = &v->sarr[k];	// everytime a scalar variable is found ANYWHERE
	else
		v->AddressOfDoubleVar = &v->arr[k];	    // everytime an array variable is found ANYWHERE
	return v;
}


//*****************************************************************************

long inot(long i)
{
	return (!i);
}

//*****************************************************************************

valrec factor(struct LOC_exec* LINK)
{
	long i, j;
	double TmpDouble;
	FILE* FileNumber;
	varrec* v;
	tokenrec* facttok;
	valrec n;
	char Str1[2048] = { 0 };
	char Tmp1[2048] = { 0 };
	char Tmp2[2048] = { 0 };

	if (LINK->Token == NULL)
		SyntaxError();
	facttok = LINK->Token;
	LINK->Token = LINK->Token->next;
	n.IsStringVal = FALSE;

	//************************************************************

	switch (facttok->kind)
	{

	case toknum:
		n.val = facttok->num;
		break;

	case tokstr:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);	// changed from calloc(255,1) by Kevin Diggins
		strcpy(n.sval, facttok->sp);
		break;



	case tokvar:
		LINK->Token = facttok;

		// Check if this is a function call
		if (LINK->Token->next != NULL && LINK->Token->next->kind == toklp)
		{
			funcrec* f = FindFunction(LINK->Token->vp->name);

			if (f != NULL)
			{
				// This IS a function call
				varrec* saved_vars[MAXDIMS];
				double saved_vals[MAXDIMS];
				char* saved_strs[MAXDIMS];
				tokenrec* saved_tok;
				valrec result;
				int k;

				LINK->Token = LINK->Token->next;  // Skip function name
				RequireToken(toklp, LINK);  // Require opening parenthesis

				// Check for immediate closing parenthesis (no arguments)
				if (LINK->Token != NULL && LINK->Token->kind == tokrp) {
					LINK->Token = LINK->Token->next;  // Skip closing parenthesis
					// No parameters to process
				}
				else {
					// Parse and evaluate arguments
					for (k = 0; k < f->numparams; k++) {
						varrec* v1 = varbase;

						// Find or create parameter variable
						while (v1 != NULL && _stricmp(v1->name, f->paramnames[k]))
							v1 = v1->next;

						if (v1 == NULL) {
							v1 = (varrec*)safe_calloc(1, sizeof(varrec));
							v1->next = varbase;
							varbase = v1;
							strcpy(v1->name, f->paramnames[k]);
							v1->numdims = 0;
							v1->IsStringVar = (f->paramnames[k][strlen(f->paramnames[k]) - 1] == '$');
							if (v1->IsStringVar) {
								v1->StringVar = NULL;
								v1->AddressOfStringVar = &v1->StringVar;
							}
							else {
								v1->DoubleVar = 0.0;
								v1->AddressOfDoubleVar = &v1->DoubleVar;
							}
						}

						saved_vars[k] = v1;

						// Save old value
						if (v1->IsStringVar) {
							saved_strs[k] = v1->StringVar;
							v1->StringVar = NULL;
						}
						else {
							saved_vals[k] = v1->DoubleVar;
						}

						// Evaluate argument
						if (v1->IsStringVar) {
							v1->StringVar = StringExpression(LINK);
						}
						else {
							v1->DoubleVar = DoubleExpression(LINK);
						}

						if (k < f->numparams - 1)
							RequireToken(tokcomma, LINK);
					}
					RequireToken(tokrp, LINK);
				}

				// Execute function body
				saved_tok = LINK->Token;
				LINK->Token = f->body;

				result = Expression(LINK);

				LINK->Token = saved_tok;

				// Restore parameter variables
				for (k = 0; k < f->numparams; k++) {
					if (saved_vars[k]->IsStringVar) {
						if (saved_vars[k]->StringVar != NULL)
							free(saved_vars[k]->StringVar);
						saved_vars[k]->StringVar = saved_strs[k];
					}
					else {
						saved_vars[k]->DoubleVar = saved_vals[k];
					}
				}

				n = result;
				break;
			}
		}

		// Not a function, treat as variable
		v = findvar(LINK);
		n.IsStringVal = v->IsStringVar;
		if (n.IsStringVal)
		{
			n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);

			if (*v->AddressOfStringVar != NULL)
			{
				strcpy(n.sval, *v->AddressOfStringVar);
			}
			else
			{
				strcpy(n.sval, "");
			}
		}
		else
			n.val = *v->AddressOfDoubleVar;
		break;




	case toklp:
		n = Expression(LINK);
		RequireToken(tokrp, LINK);
		break;

	case tokminus:
		n.val = -Doublefactor(LINK);
		break;

	case tokplus:
		n.val = Doublefactor(LINK);
		break;

		//*****************************************************************************
		//      DO NOT REQUIRE () on non-string functions - leave it to the user
		//*****************************************************************************

	case tokkeypress:
		n.val = keypress();
		break;

	case toktimer:
		n.val = (float)GetTickCount() * 0.001;
		break;

	case tokrnd:
		n.val = (float)rand() / RAND_MAX;
		break;

	case toknot:
		n.val = inot(intfactor(LINK));
		break;

	case toksqr:
		TmpDouble = Doublefactor(LINK);
		n.val = TmpDouble * TmpDouble;
		break;

	case toksqrt:
		n.val = sqrt(Doublefactor(LINK));
		break;

	case tokround:
		RequireToken(toklp, LINK);
		n.val = DoubleExpression(LINK);
		RequireToken(tokcomma, LINK);
		i = IntegerExpression(LINK);
		n.val = Round(n.val, i);
		RequireToken(tokrp, LINK);
		break;

	case toksin:
		n.val = sin(Doublefactor(LINK));
		break;

	case tokcos:
		n.val = cos(Doublefactor(LINK));
		break;

	case toktan:
		n.val = Doublefactor(LINK);
		n.val = sin(n.val) / cos(n.val);
		break;

	case tokatan:
		n.val = atan(Doublefactor(LINK));
		break;

	case toklog10:
		n.val = log10(Doublefactor(LINK));
		break;

	case toklog:
		n.val = log(Doublefactor(LINK));
		break;

	case tokexp:
		n.val = exp(Doublefactor(LINK));
		break;

	case tokabs:
		n.val = fabs(Doublefactor(LINK));
		break;

	case toksgn:
		n.val = Doublefactor(LINK);
		n.val = (n.val > 0) - (n.val < 0);
		break;

	case tokint:
		n.val = Doublefactor(LINK);
		n.val = floor(n.val);
		break;

	case toklike:
		RequireToken(toklp, LINK);
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.sval = StringExpression(LINK);
		n.val = like(Tmp1, n.sval);
		RequireToken(tokrp, LINK);
		break;

	case tokverify:
		RequireToken(toklp, LINK);
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.sval = StringExpression(LINK);
		n.val = Verify(Tmp1, n.sval);
		RequireToken(tokrp, LINK);
		break;

	case tokexist:
		RequireToken(toklp, LINK);
		n.sval = StringExpression(LINK);
		n.val = Exist(n.sval);
		RequireToken(tokrp, LINK);
		break;

	case tokstr_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		NumToStr(n.sval, Doublefactor(LINK));
		RequireToken(tokrp, LINK);
		break;

	case tokhex_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		sprintf(n.sval, "%lX", intfactor(LINK));
		RequireToken(tokrp, LINK);
		break;

	case tokval:
		RequireToken(toklp, LINK);
		n.sval = StringExpression(LINK);
		n.val = (double)atof(n.sval);
		RequireToken(tokrp, LINK);
		break;

	case toklof:
		RequireToken(toklp, LINK);
		n.sval = StringExpression(LINK);
		n.val = lof(n.sval);
		RequireToken(tokrp, LINK);
		break;

	case tokenviron_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		GetEnvironmentVariable(n.sval, Str1, MAXSTRINGVAR);
		strcpy(n.sval, Str1);
		RequireToken(tokrp, LINK);
		break;

	case tokchr_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		strcpy(n.sval, " ");
		n.sval[0] = (char)intfactor(LINK);
		RequireToken(tokrp, LINK);
		break;

	case toktime_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		strcpy(n.sval, timef());
		break;

	case tokinkey_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		strcpy(n.sval, inkey());
		break;

	case tokcrlf_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(5, 1);
		strcpy(n.sval, crlf());
		break;

	case tokdate_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		strcpy(n.sval, mystrDate(DateStr));
		break;

	case tokwindir_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		strcpy(n.sval, windir());
		break;

	case tokcurdir_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		strcpy(n.sval, curdir());
		break;

	case toksysdir_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		strcpy(n.sval, sysdir());
		break;

	case toktempdir_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		strcpy(n.sval, tempdir());
		break;

	case toktrim_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, trim(n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokltrim_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, ltrim(n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokrtrim_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, rtrim(n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokucase_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, ucase(n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokfindfirst_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, findfirst(n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokfindnext_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		strcpy(n.sval, findnext());
		break;

	case toklcase_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, lcase(n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokmcase_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, mcase(n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokspace_:
		n.IsStringVal = TRUE;
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
		RequireToken(toklp, LINK);
		i = IntegerExpression(LINK);
		if (i < 1)
			i = 1;
		strcpy(n.sval, space(i));
		RequireToken(tokrp, LINK);
		break;

	case tokleft_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		RequireToken(tokcomma, LINK);
		i = IntegerExpression(LINK);
		if (i < 1)
			i = 1;
		strcpy(n.sval, left(n.sval, i));
		RequireToken(tokrp, LINK);
		break;

	case tokusing_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		RequireToken(tokcomma, LINK);
		TmpDouble = DoubleExpression(LINK);
		RequireToken(tokrp, LINK);
		strcpy(n.sval, Using(n.sval, TmpDouble));
		break;

	case tokright_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		RequireToken(tokcomma, LINK);
		i = IntegerExpression(LINK);
		if (i < 1)
			i = 1;
		strcpy(n.sval, right(n.sval, i));
		RequireToken(tokrp, LINK);
		break;

	case tokrepeat_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		RequireToken(tokcomma, LINK);
		i = IntegerExpression(LINK);
		if (i < 1)
			i = 1;
		strcpy(n.sval, repeat(n.sval, i));
		RequireToken(tokrp, LINK);
		break;

	case tokextract_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, extract(Tmp1, n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokinstr:
		RequireToken(toklp, LINK);
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.sval = StringExpression(LINK);
		RequireToken(tokrp, LINK);
		n.val = instr(Tmp1, n.sval);
		break;

	case tokireplace_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(Tmp2, n.sval);
		RequireToken(tokcomma, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, iReplace(Tmp1, Tmp2, n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokinputbox:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(Tmp2, n.sval);
		RequireToken(tokcomma, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, InputBox(Tmp1, Tmp2, n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokyncancel:
		n.IsStringVal = FALSE;
		RequireToken(toklp, LINK);
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.sval = StringExpression(LINK);
		n.val = YN_CANCEL(Tmp1, n.sval);
		RequireToken(tokrp, LINK);
		break;

	case tokenc_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		RequireToken(tokrp, LINK);
		strcpy(n.sval, enc(n.sval));
		break;

	case tokremain_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, remain(Tmp1, n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokretain_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, retain(Tmp1, n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokremove_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(Tmp1, n.sval);
		RequireToken(tokcomma, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		strcpy(n.sval, RemoveStr(Tmp1, n.sval));
		RequireToken(tokrp, LINK);
		break;

	case tokasc:
		RequireToken(toklp, LINK);
		n.sval = strfactor(LINK);
		if (*n.sval == '\0')
			n.val = 0.0;
		else
			n.val = n.sval[0];
		RequireToken(tokrp, LINK);
		break;

	case tokmid_:
		RequireToken(toklp, LINK);
		n.IsStringVal = TRUE;
		n.sval = StringExpression(LINK);
		RequireToken(tokcomma, LINK);

		i = IntegerExpression(LINK);

		if (i < 1)
			i = 1;

		j = (int)strlen(n.sval);

		if (LINK->Token != NULL && LINK->Token->kind == tokcomma)
		{
			LINK->Token = LINK->Token->next;
			j = IntegerExpression(LINK);
		}

		if (j > (int)strlen(n.sval) - i + 1)
			j = (int)strlen(n.sval) - i + 1;

		if (i > (int)strlen(n.sval))
			*n.sval = '\0';
		else
		{
			sprintf(Str1, "%.*s", (int)j, n.sval + i - 1);
			strcpy(n.sval, Str1);
		}

		RequireToken(tokrp, LINK);
		break;

	case toklen:
		RequireToken(toklp, LINK);
		n.sval = strfactor(LINK);
		n.val = (int)strlen(n.sval);
		RequireToken(tokrp, LINK);
		break;

	case tokeof:
		FileNumber = GetFilePtr(intfactor(LINK));
		n.val = EoF(FileNumber);
		break;

	case tokpi:
		n.val = 3.14159265358979;
		break;

	case toktrue:
		n.val = TRUE;
		break;

	case tokfalse:
		n.val = FALSE;
		break;

	default:
		SyntaxError();
		break;
	}
	return n;
}
//****************************************************************

valrec upexpr(struct LOC_exec* LINK)
{
	valrec n, n2;

	n = factor(LINK);

	while (LINK->Token != NULL && LINK->Token->kind == tokup)
	{
		if (n.IsStringVal)
			MismatchError();
		LINK->Token = LINK->Token->next;
		n2 = upexpr(LINK);
		if (n2.IsStringVal)
			MismatchError();
		if (n.val >= 0)
		{
			n.val = exp(n2.val * log(n.val));
			continue;
		}
		if (n2.val != (long)n2.val)
			n.val = log(n.val);
		n.val = exp(n2.val * log(-n.val));
		if (((long)n2.val) & 1)
			n.val = -n.val;
	}
	return n;
}

//*****************************************************************************

valrec term(struct LOC_exec* LINK)
{
	valrec n, n2;
	long k;

	n = upexpr(LINK);

	while (LINK->Token != NULL && LINK->Token->kind < 32 && ((1L << (LINK->Token->kind)) & ((1L << ((long)toktimes)) | (1L << ((long)tokdiv)) | (1L << ((long)tokmod)))) != 0)
	{
		k = LINK->Token->kind;
		LINK->Token = LINK->Token->next;
		n2 = upexpr(LINK);
		if (n.IsStringVal || n2.IsStringVal)
			MismatchError();
		if (k == tokmod)
		{
			n.val = (long)floor(n.val + 0.5) % (long)floor(n2.val + 0.5);
		}
		else if (k == toktimes)
			n.val *= n2.val;
		else
			n.val /= n2.val;
	}
	return n;
}

//*****************************************************************************

valrec sexpr(struct LOC_exec* LINK)
{
	valrec n, n2;
	long k;

	n = term(LINK);

	while (LINK->Token != NULL && LINK->Token->kind < 32 && ((1L << (LINK->Token->kind)) & ((1L << (tokplus)) | (1L << ((long)tokminus)))) != 0)
	{
		k = LINK->Token->kind;

		LINK->Token = LINK->Token->next;

		n2 = term(LINK);

		if (n.IsStringVal != n2.IsStringVal)
			MismatchError();

		if (k == tokplus)
		{
			if (n.IsStringVal)
			{
				strcat(n.sval, n2.sval);
				free(n2.sval);
			}
			else
				n.val += n2.val;
		}
		else
		{
			if (n.IsStringVal)
				MismatchError();
			else
				n.val -= n2.val;
		}
	}
	return n;
}

//*****************************************************************************

valrec relexpr(struct LOC_exec* LINK)
{
	valrec n, n2;
	BOOL f;
	long k;

	n = sexpr(LINK);

	while (LINK->Token != NULL && LINK->Token->kind < 32 && ((1L << (LINK->Token->kind)) & ((1L << ((long)tokne + 1)) - (1L << ((long)tokeq)))) != 0)
	{
		k = LINK->Token->kind;
		LINK->Token = LINK->Token->next;
		n2 = sexpr(LINK);
		if (n.IsStringVal != n2.IsStringVal)
			MismatchError();
		if (n.IsStringVal)
		{
			f = ((!strcmp(n.sval, n2.sval) && (unsigned long)k < 32 && ((1L << ((long)k)) & ((1L << ((long)tokeq)) | (1L << ((long)tokge)) | (1L << ((long)tokle)))) != 0) || (strcmp(n.sval, n2.sval) < 0 && (unsigned long)k < 32 && ((1L << ((long)k)) & ((1L << ((long)toklt)) | (1L << ((long)tokle)) | (1L << ((long)tokne)))) != 0) || (strcmp(n.sval, n2.sval) > 0 && (unsigned long)k < 32 && ((1L << ((long)k)) & ((1L << ((long)tokgt)) | (1L << ((long)tokge)) | (1L << ((long)tokne)))) != 0));
			free(n.sval);
			free(n2.sval);
		}
		else
		{
			f = ((n.val == n2.val && (unsigned long)k < 32 && ((1L << ((long)k)) & ((1L << ((long)tokeq)) | (1L << ((long)tokge)) | (1L << ((long)tokle)))) != 0) || (n.val < n2.val && (unsigned long)k < 32 && ((1L << ((long)k)) & ((1L << ((long)toklt)) | (1L << ((long)tokle)) | (1L << ((long)tokne)))) != 0) || (n.val > n2.val && (unsigned long)k < 32 && ((1L << ((long)k)) & ((1L << ((long)tokgt)) | (1L << ((long)tokge)) | (1L << ((long)tokne)))) != 0));
		}
		n.IsStringVal = FALSE;
		n.val = f;
	}
	return n;
}

//*****************************************************************************

valrec andexpr(struct LOC_exec* LINK)
{
	valrec n, n2;
	n = relexpr(LINK);
	while (LINK->Token != NULL && LINK->Token->kind == tokand)
	{
		LINK->Token = LINK->Token->next;
		n2 = relexpr(LINK);
		if (n.IsStringVal || n2.IsStringVal)
			MismatchError();
		n.val = ((long)n.val) & ((long)n2.val);
	}
	return n;
}

//*****************************************************************************

valrec Expression(struct LOC_exec* LINK)
{
	valrec n, n2;
	long k;
	n = andexpr(LINK);
	while (LINK->Token != NULL && LINK->Token->kind < 32 && ((1L << (LINK->Token->kind)) & ((1L << ((long)tokor)) | (1L << ((long)tokxor)))) != 0)
	{
		k = LINK->Token->kind;
		LINK->Token = LINK->Token->next;
		n2 = andexpr(LINK);

		if (n.IsStringVal || n2.IsStringVal)
			MismatchError();

		if (k == tokor)
			n.val = ((long)n.val) | ((long)n2.val);
		else
			n.val = ((long)n.val) ^ ((long)n2.val);
	}
	return n;
}

//*****************************************************************************

void checkextra(struct LOC_exec* LINK)
{
	if (LINK->Token != NULL)
		Abort("Extra information on line");
}

//*****************************************************************************

BOOL IsEndOfStatement(struct LOC_exec* LINK)
{
	return (LINK->Token == NULL || LINK->Token->kind == tokelse || LINK->Token->kind == tokcolon || LINK->Token->kind == tokrem);
}

//*****************************************************************************

void skiptoeos(struct LOC_exec* LINK)
{
	while (!IsEndOfStatement(LINK))
		LINK->Token = LINK->Token->next;
}

//*****************************************************************************

linerec* FindTargetLineNumber(long n)
{
	linerec* l;

	l = linebase;
	while (l != NULL && l->num != n)
		l = l->next;

	if (l == NULL)
		Abort("Missing goto/gosub target");
	return l;
}

//*****************************************************************************

void cmdend(struct LOC_exec* LINK)
{
	stmtline = NULL;
	LINK->Token = NULL;
}

//*****************************************************************************

void cmdnew(struct LOC_exec* LINK)
{
	LPVOID p;
	cmdend(LINK);
	ClearLoops();
	ClearVars();
	RestoreData();
	strcpy(OurLoadedFname, "");

	while (linebase != NULL)
	{
		p = (LPVOID)linebase->next;
		DisposeTokens(&linebase->txt);
		free(linebase);
		linebase = (linerec*)p;
	}

	while (varbase != NULL)
	{
		p = (LPVOID)varbase->next;
		if (varbase->IsStringVar)
		{
			if (*varbase->AddressOfStringVar != NULL)
				free(*varbase->AddressOfStringVar);
		}
		free(varbase);
		varbase = (varrec*)p;
	}

	// Clear user-defined functions
	while (funcbase != NULL)
	{
		p = (LPVOID)funcbase->next;
		DisposeTokens(&funcbase->body);
		free(funcbase);
		funcbase = (funcrec*)p;
	}


	//**************
	SetConsoleTitle("Gillespie BASIC for Windows by Kevin Diggins");
	color(FGCOLOR, BGCOLOR);
	cls();
	//**************
}

//*****************************************************************************

void cmdokcancel(struct LOC_exec* LINK)
{
	char* Title;
	char* Message;

	Title = (char*)safe_calloc(MAXSTRINGVAR, 1);
	Message = (char*)safe_calloc(MAXSTRINGVAR, 1);

	Title = StringExpression(LINK);
	RequireToken(tokcomma, LINK);
	Message = StringExpression(LINK);

	OK_CANCEL(Title, Message);
	free(Title);
	free(Message);

}

//*****************************************************************************

void cmdlist(struct LOC_exec* LINK)
{
	linerec* l;
	long n1, n2;
	int ii;

	for (ii = 1; ii <= 100; ii++)
	{
		BumpDown();
	}
	//**************
	color(FGCOLOR, BGCOLOR);
	cls();
	//**************
	do
	{
		n1 = 0;
		n2 = MAXLINENUMBER;

		if (LINK->Token != NULL && LINK->Token->kind == toknum)	// example:  LIST 350
		{
			n1 = (long)LINK->Token->num;
			LINK->Token = LINK->Token->next;
			if (LINK->Token == NULL || LINK->Token->kind != tokminus)
				n2 = n1;
		}

		if (LINK->Token != NULL && LINK->Token->kind == tokminus)	// example:  LIST 350-400
		{
			LINK->Token = LINK->Token->next;
			if (LINK->Token != NULL && LINK->Token->kind == toknum)
			{
				n2 = (long)LINK->Token->num;
				LINK->Token = LINK->Token->next;
			}
			else
				n2 = MAXLINENUMBER;
		}

		l = linebase;

		while (l != NULL && l->num <= n2)
		{
			if (l->num >= n1)
			{
				printf("%ld ", l->num);	// this displays the line numbers
				ListTokens(stdout, l->txt);
				putchar('\n');
			}
			l = l->next;
		}
		if (!IsEndOfStatement(LINK))
			RequireToken(tokcomma, LINK);
	} while (!IsEndOfStatement(LINK));
}

//*****************************************************************************

void cmdrun(struct LOC_exec* LINK)
{
	linerec* l;
	long i;
	char s[2048] = { 0 };
	l = linebase;
	if (!IsEndOfStatement(LINK))
	{
		if (LINK->Token->kind == toknum)
			l = FindTargetLineNumber(IntegerExpression(LINK));
		else
		{
			StringExpressionEx(s, LINK);
			i = 0;
			if (!IsEndOfStatement(LINK))
			{
				RequireToken(tokcomma, LINK);
				i = IntegerExpression(LINK);
			}
			checkextra(LINK);
			cmdload(FALSE, s, LINK);
			if (i == 0)
				l = linebase;
			else
				l = FindTargetLineNumber(i);
		}
	}
	stmtline = l;
	LINK->gotoflag = TRUE;
	ClearVars();
	ClearLoops();
	RestoreData();
}

//*****************************************************************************

void cmdload(BOOL merging, char* name, struct LOC_exec* LINK)
{
	FILE* f;
	tokenrec* buf;
	char Str1[255] = { 0 };
	long LineNumber = 0;

	f = NULL;

	if (!instr(name, "."))
		sprintf(Str1, "%s.bas", name);
	else
		sprintf(Str1, "%s", name);

	//***********************************************************************************
	if (Str1[0] == 46)	//LOAD "" produces Str1 = ".bas"   46 = '.'
	{
		Str1[0] = 0;
		strcpy(Str1, GetFileName("Load", "*.bas"));
		if (Str1[0] == 0)
			goto bailout;	// cancelled out of GetFileName dlg
	}
	//***********************************************************************************
	if (!Exist(Str1))
	{
		printf("LOAD error: File not found.\n");
		goto bailout2;
	}

	if (!merging)
		cmdnew(LINK);

	strcpy(OurLoadedFname, Str1);

	f = fopen(Str1, "r");

	if (f == NULL)
	{
		printf("LOAD error: File not found.\n");
		goto bailout2;
	}

	while (fgets(G_inbuf, 2047, f) != NULL)
	{
		if (G_inbuf[(int)strlen(G_inbuf) - 1] == 10)
			G_inbuf[(int)strlen(G_inbuf) - 1] = 0;
		//-------------------------------------------------------
		// we can do some PRE-PROCESSING of inbuf at this stage
		// Let's add line numbers to files without them
		//-------------------------------------------------------
		if (!IsNumber(left(G_inbuf, 1)))
		{
			LineNumber += 10;
			strcpy(G_inbuf, join(3, str(LineNumber), " ", G_inbuf));
		}
		//-------------------------------------------------------
		ParseInput(&buf);

		if (curline == 0)
		{
			printf("Bad line in file\n");
			DisposeTokens(&buf);
		}
	}

	if (f != NULL)
		fclose(f);
	f = NULL;

	SetConsoleTitle(Str1);

bailout:
	;
	cmdlist(LINK);

bailout2:
	;
}

//*****************************************************************************

void cmdsave(struct LOC_exec* LINK)
{
	FILE* f;
	linerec* l;
	char Str1[255] = { 0 };
	char Str2[255] = { 0 };

	if ((int)strlen(OurLoadedFname) > 0)
		strcpy(Str2, OurLoadedFname);
	else
		sprintf(Str2, "%s.Bas", StringExpressionEx(Str1, LINK));

	strcpy(OurLoadedFname, Str2);

	f = fopen(Str2, "w");

	if (f == NULL)
	{
		printf("SAVE error: File not saved.\n");
		goto bailout;
	}

	l = linebase;

	while (l != NULL)
	{
		fprintf(f, "%ld ", l->num);
		ListTokens(f, l->txt);
		putc('\n', f);
		l = l->next;
	}
	if (f != NULL)
		fclose(f);
	f = NULL;
	//-------------------------------------------------------
	if ((int)strlen(OurLoadedFname) == 0)
		sprintf(Str1, "%s%s%s", curdir(), "\\", Str2);
	else
		strcpy(Str1, OurLoadedFname);
	//-------------------------------------------------------
	printf("%s%s", "File saved: ", OurLoadedFname);
	SetConsoleTitle(Str1);
bailout:
	;
}

//****************************************************************

void cmdbye(void)
{
	color(7, 0);
	cls();
	locate(1, 1);
	exitflag = TRUE;
}

//****************************************************************

void cmddel(struct LOC_exec* LINK)
{
	linerec* l, * l0, * l1;
	long n1, n2;

	do
	{
		if (IsEndOfStatement(LINK))
			SyntaxError();
		n1 = 0;
		n2 = MAXLINENUMBER;
		if (LINK->Token != NULL && LINK->Token->kind == toknum)
		{
			n1 = (long)LINK->Token->num;
			LINK->Token = LINK->Token->next;
			if (LINK->Token == NULL || LINK->Token->kind != tokminus)
				n2 = n1;
		}
		if (LINK->Token != NULL && LINK->Token->kind == tokminus)
		{
			LINK->Token = LINK->Token->next;
			if (LINK->Token != NULL && LINK->Token->kind == toknum)
			{
				n2 = (long)LINK->Token->num;
				LINK->Token = LINK->Token->next;
			}
			else
				n2 = MAXLINENUMBER;
		}
		l = linebase;
		l0 = NULL;
		while (l != NULL && l->num <= n2)
		{
			l1 = l->next;
			if (l->num >= n1)
			{
				if (l == stmtline)
				{
					cmdend(LINK);
					ClearLoops();
					RestoreData();
				}
				if (l0 == NULL)
					linebase = l->next;
				else
					l0->next = l->next;
				DisposeTokens(&l->txt);
				free(l);
			}
			else
				l0 = l;
			l = l1;
		}
		if (!IsEndOfStatement(LINK))
			RequireToken(tokcomma, LINK);
	} while (!IsEndOfStatement(LINK));
}

//*****************************************************************************

void cmdrenum(struct LOC_exec* LINK)
{
	linerec* l, * l1;
	tokenrec* tok;
	long lnum, step;

	lnum = 10;
	step = 10;
	if (!IsEndOfStatement(LINK))
	{
		lnum = IntegerExpression(LINK);
		if (!IsEndOfStatement(LINK))
		{
			RequireToken(tokcomma, LINK);
			step = IntegerExpression(LINK);
		}
	}
	l = linebase;
	if (l == NULL)
		return;
	while (l != NULL)
	{
		l->num2 = lnum;
		lnum += step;
		l = l->next;
	}
	l = linebase;
	do
	{
		tok = l->txt;
		do
		{
			if (tok->kind == tokdel || tok->kind == tokrestore || tok->kind == toklist || tok->kind == tokrun || tok->kind == tokelse || tok->kind == tokthen || tok->kind == tokgosub || tok->kind == tokgoto)
			{

				while (tok->next != NULL && tok->next->kind == toknum)
				{
					tok = tok->next;
					lnum = (long)floor(tok->num + 0.5);
					l1 = linebase;
					while (l1 != NULL && l1->num != lnum)
						l1 = l1->next;
					if (l1 == NULL)
						printf("Undefined line %ld in line %ld\n", lnum, l->num2);
					else
						tok->num = l1->num2;
					if (tok->next != NULL && tok->next->kind == tokcomma)
						tok = tok->next;
				}
			}
			tok = tok->next;
		} while (tok != NULL);
		l = l->next;
	} while (l != NULL);
	l = linebase;
	while (l != NULL)
	{
		l->num = l->num2;
		l = l->next;
	}
}

//*****************************************************************************

void cmdprint(struct LOC_exec* LINK)
{
	BOOL semiflag;
	valrec n;
	char Str1[2048] = { 0 };
	semiflag = FALSE;

	while (!IsEndOfStatement(LINK))
	{
		semiflag = FALSE;
		if ((unsigned long)LINK->Token->kind < 32 && ((1L << ((long)LINK->Token->kind)) & ((1L << ((long)toksemi)) | (1L << ((long)tokcomma)))) != 0)
		{
			semiflag = TRUE;
			LINK->Token = LINK->Token->next;
			continue;
		}
		n = Expression(LINK);
		if (n.IsStringVal)
		{
			fputs(n.sval, stdout);
			free(n.sval);
		}
		else
			printf(" %s ", NumToStr(Str1, n.val));
	}
	if (!semiflag)
		putchar('\n');
}
//*****************************************************************************

void cmdsleep(struct LOC_exec* LINK)
{
	DWORD milliseconds;
	milliseconds = IntegerExpression(LINK);
	Sleep(milliseconds);
}

//*****************************************************************************

void cmdtextmode(struct LOC_exec* LINK)
{
	long textlines;
	textlines = IntegerExpression(LINK);
	TextMode(textlines);
}
//*****************************************************************************

void cmdrandomize(struct LOC_exec* LINK)
{
	long seed;
	seed = IntegerExpression(LINK);
	srand(seed);
}
//*****************************************************************************

void cmdsetcursor(struct LOC_exec* LINK)
{
	int showhide;
	showhide = IntegerExpression(LINK);
	Cursorsh(showhide);
}

//*****************************************************************************

void cmdclose(struct LOC_exec* LINK)
{
	FILE* FileNumber;
	int ioNumber;
	ioNumber = IntegerExpression(LINK);
	FileNumber = GetFilePtr(ioNumber);
	fflush(FileNumber);
	fclose(FileNumber);
	UnSetFilePtr(ioNumber);
}

//*****************************************************************************

void cmdrewind(struct LOC_exec* LINK)
{
	FILE* FileNumber;
	int ioNumber;
	ioNumber = IntegerExpression(LINK);
	FileNumber = GetFilePtr(ioNumber);
	rewind(FileNumber);
}

//*****************************************************************************

void cmdfprint(struct LOC_exec* LINK)
{
	FILE* Channel;
	BOOL semiflag;
	valrec n;
	char Str1[2048] = { 0 };
	semiflag = FALSE;

	Channel = GetFilePtr(IntegerExpression(LINK));
	RequireToken(tokcomma, LINK);

	while (!IsEndOfStatement(LINK))
	{
		semiflag = FALSE;
		if ((unsigned long)LINK->Token->kind < 32 && ((1L << ((long)LINK->Token->kind)) & ((1L << ((long)toksemi)) | (1L << ((long)tokcomma)))) != 0)
		{
			semiflag = TRUE;
			LINK->Token = LINK->Token->next;
			continue;
		}

		n = Expression(LINK);
		if (n.IsStringVal)
		{
			fprintf(Channel, "%s ", n.sval);
			free(n.sval);
		}
		else
			fprintf(Channel, "%s ", NumToStr(Str1, n.val));
	}
	if (!semiflag)
		fprintf(Channel, "%s", "\n");
}

//*****************************************************************************

void cmdget(struct LOC_exec* LINK)	// GET fp1,A$,numofbytes
{
	FILE* FileNumber;
	varrec* v;
	int numbytes;

	char* s = (char*)safe_calloc(MAXSTRINGVAR, 1);

	FileNumber = GetFilePtr(IntegerExpression(LINK));

	if (FileNumber == NULL)
		Abort("File Not Open");

	RequireToken(tokcomma, LINK);

	v = findvar(LINK);

	RequireToken(tokcomma, LINK);
	numbytes = IntegerExpression(LINK);

	GET(FileNumber, s, numbytes);

	if (*v->AddressOfStringVar != NULL)
		free(*v->AddressOfStringVar);
	*v->AddressOfStringVar = (char*)safe_calloc(MAXSTRINGVAR, 1);
	strcpy(*v->AddressOfStringVar, s);
	free(s);
}

//*****************************************************************************

void cmdput(struct LOC_exec* LINK)	// PUT fp1,A$,numofbytes
{
	FILE* FileNumber;
	valrec n;
	int numbytes;

	char* s = (char*)safe_calloc(MAXSTRINGVAR, 1);

	FileNumber = GetFilePtr(IntegerExpression(LINK));

	if (FileNumber == NULL)
	{
		free(s);
		Abort("File Not Open");
	}

	RequireToken(tokcomma, LINK);

	n = factor(LINK);
	if (!n.IsStringVal)
		MismatchError();
	strcpy(s, n.sval);

	RequireToken(tokcomma, LINK);
	numbytes = IntegerExpression(LINK);

	PUT(FileNumber, s, numbytes);

	fflush(FileNumber);

	free(s);
}

//*****************************************************************************

void cmdseek(struct LOC_exec* LINK)	// PUT fp1,A$,numofbytes
{
	FILE* FileNumber;
	int numbytes;

	FileNumber = GetFilePtr(IntegerExpression(LINK));

	if (FileNumber == NULL)
		Abort("File Not Open");

	RequireToken(tokcomma, LINK);

	numbytes = IntegerExpression(LINK);

	fseek(FileNumber, numbytes, 0);
}

//*****************************************************************************

void cmdflineinput(struct LOC_exec* LINK)	//LINE INPUT handler
{
	FILE* FileNumber;
	varrec* v;
	char* s = (char*)safe_calloc(MAXSTRINGVAR, 1);

	RequireToken(tokinput, LINK);	//swallow the INPUT keyword

	FileNumber = GetFilePtr(IntegerExpression(LINK));

	if (FileNumber == NULL)
		Abort("File Not Open");

	RequireToken(tokcomma, LINK);

	v = findvar(LINK);

	fgets(s, 2047, FileNumber);
	s[(int)strlen(s) - 1] = 0;

	if (*v->AddressOfStringVar != NULL)
		free(*v->AddressOfStringVar);
	*v->AddressOfStringVar = (char*)safe_calloc(MAXSTRINGVAR, 1);
	strcpy(*v->AddressOfStringVar, s);
	free(s);
}

//*****************************************************************************

static void cmdopen(struct LOC_exec* LINK)
{
	char* FileName;
	int FileNumber = 0;
	int Mode = 0;
	FileName = (char*)safe_calloc(MAXSTRINGVAR, 1);
	FileName = StringExpression(LINK);
	RequireToken(tokfor, LINK);

	if (LINK->Token->kind != tokinput && LINK->Token->kind != tokoutput && LINK->Token->kind != tokappend && LINK->Token->kind != tokbinary)
		SyntaxError();

	Mode = LINK->Token->kind;
	LINK->Token = LINK->Token->next;
	RequireToken(tokas, LINK);
	FileNumber = IntegerExpression(LINK);
	UnSetFilePtr(FileNumber);	// only one open file per FileNumber

	if (Mode == tokinput)
		OpenFileForReading(FileNumber, FileName);
	if (Mode == tokoutput)
		OpenFileForWriting(FileNumber, FileName);
	if (Mode == tokappend)
		OpenFileForAppending(FileNumber, FileName);
	if (Mode == tokbinary)
		OpenFileForBinary(FileNumber, FileName);

	free(FileName);
}

//****************************************************************

void cmdswap(struct LOC_exec* LINK)
{
	int BothAreScalar = 0;	// must be 0 or 2 - otherwise an error
	int BothAreStrings = 0;	// must be 0 or 2 - otherwise an error
	double* AA = NULL;
	double* BB = NULL;
	char* sAA = NULL;
	char* sBB = NULL;
	varrec* v;
	//********************************************
	v = findvar(LINK);
	if (v->IsStringVar)
		BothAreStrings++;
	if (v->numdims == 0)
		BothAreScalar++;
	//********************************************
	if (v->IsStringVar && v->numdims == 0)
		sAA = v->StringVar;	//simple string
	if (v->IsStringVar && v->numdims > 0)
		sAA = (char*)v->AddressOfStringVar;	//array string
	if (!v->IsStringVar && v->numdims == 0)
		AA = &v->DoubleVar;	//simple number
	if (!v->IsStringVar && v->numdims > 0)
		AA = (double*)v->AddressOfDoubleVar;	//array number
	//********************************************
	RequireToken(tokcomma, LINK);
	v = findvar(LINK);
	if (v->IsStringVar)
		BothAreStrings++;
	if (v->numdims == 0)
		BothAreScalar++;
	//********************************************
	if (v->IsStringVar && v->numdims == 0)
		sBB = v->StringVar;	//simple string
	if (v->IsStringVar && v->numdims > 0)
		sBB = (char*)v->AddressOfStringVar;	//array string
	if (!v->IsStringVar && v->numdims == 0)
		BB = &v->DoubleVar;	//simple number
	if (!v->IsStringVar && v->numdims > 0)
		BB = (double*)v->AddressOfDoubleVar;	//array number
	//********************************************
	if ((BothAreStrings == 2) && (sAA == NULL || sBB == NULL))
		Abort("SWAP detected a NULL string");
	//********************************************
	if (BothAreStrings == 1)
		Abort("SWAP detected mixing variable types");
	if (BothAreScalar == 1)
		Abort("SWAP detected mixing array and scalar variables");
	//********************************************
	if (!BothAreStrings)
		swap((char*)AA, (char*)BB, sizeof(double));
	if (BothAreStrings && BothAreScalar)
		swap(sAA, sBB, MAXSTRINGVAR);
	if (BothAreStrings && !BothAreScalar)
		swap((char*)sAA, (char*)sBB, sizeof(char*));
}

//*****************************************************************

void cmdinput(struct LOC_exec* LINK)
{
	varrec* v;
	char* s = (char*)safe_calloc(MAXSTRINGVAR, 1);

	tokenrec* tok, * tok0, * tok1;
	BOOL strflag;

	if (LINK->Token != NULL && LINK->Token->kind == tokstr)
	{
		fputs(LINK->Token->sp, stdout);
		LINK->Token = LINK->Token->next;
		RequireToken(tokcomma, LINK);
	}
	else
		printf("? ");
	tok = LINK->Token;

	if (LINK->Token == NULL || LINK->Token->kind != tokvar)
		SyntaxError();

	strflag = LINK->Token->vp->IsStringVar;

	do
	{
		if (LINK->Token != NULL && LINK->Token->kind == tokvar)
		{
			if (LINK->Token->vp->IsStringVar != strflag)
				SyntaxError();
		}
		LINK->Token = LINK->Token->next;
	} while (!IsEndOfStatement(LINK));
	LINK->Token = tok;
	if (strflag)
	{
		do
		{
			GB_gets(G_inbuf, MAXSTRINGVAR);

			v = findvar(LINK);
			if (*v->AddressOfStringVar != NULL)
				free(*v->AddressOfStringVar);

			*v->AddressOfStringVar = (char*)safe_calloc(MAXSTRINGVAR, 1);
			strcpy(*v->AddressOfStringVar, s);
			if (!IsEndOfStatement(LINK))
			{
				RequireToken(tokcomma, LINK);
				printf("? ");
			}
		} while (!IsEndOfStatement(LINK));
		free(s);
		return;
	}
	GB_gets(s, MAXSTRINGVAR);
	Parse(s, &tok);
	tok0 = tok;
	do
	{
		v = findvar(LINK);
		while (tok == NULL)
		{
			printf("? ");
			GB_gets(s, MAXSTRINGVAR);
			DisposeTokens(&tok0);
			Parse(s, &tok);
			tok0 = tok;
		}
		tok1 = LINK->Token;
		LINK->Token = tok;
		*v->AddressOfDoubleVar = DoubleExpression(LINK);

		if (LINK->Token != NULL)
		{
			if (LINK->Token->kind == tokcomma)
				LINK->Token = LINK->Token->next;
			else
				SyntaxError();
		}
		tok = LINK->Token;

		LINK->Token = tok1;

		if (!IsEndOfStatement(LINK))
			RequireToken(tokcomma, LINK);
	} while (!IsEndOfStatement(LINK));
	free(s);
	DisposeTokens(&tok0);
}

//*******************************************************************************

void cmdlet(BOOL implied, struct LOC_exec* LINK)
{
	varrec* v;
	char* old, * gnew;
	double d_value;
	double* target;
	char** starget;
	target = NULL;
	starget = NULL;

	if (implied)
		LINK->Token = stmttok;

	v = findvar(LINK);

	if (v->IsStringVar)
	{
		starget = v->AddressOfStringVar;
	}
	else
	{
		target = v->AddressOfDoubleVar;
	}

	RequireToken(tokeq, LINK);

	if (!v->IsStringVar)
	{
		// allow arrays on both, left and right side of =

		d_value = DoubleExpression(LINK);
		v->AddressOfDoubleVar = target;
		*v->AddressOfDoubleVar = d_value;
		return;
	}
	gnew = StringExpression(LINK);
	v->AddressOfStringVar = starget;
	old = *v->AddressOfStringVar;
	*v->AddressOfStringVar = gnew;
	if (old != NULL)
		free(old);
}

//*****************************************************************************

void cmdgoto(struct LOC_exec* LINK)
{
	stmtline = FindTargetLineNumber(IntegerExpression(LINK));
	LINK->Token = NULL;
	LINK->gotoflag = TRUE;
}

//*****************************************************************************

void cmdif(struct LOC_exec* LINK)
{
	double n;
	long i;

	n = DoubleExpression(LINK);
	RequireToken(tokthen, LINK);

	if (n == 0)	// if 1st expression is logically FALSE or ZERO
	{
		i = 0;
		do
		{
			if (LINK->Token != NULL)
			{
				if (LINK->Token->kind == tokif)
					i++;
				if (LINK->Token->kind == tokelse)
					i--;
				LINK->Token = LINK->Token->next;
			}
		} while (LINK->Token != NULL && i >= 0);
	}

	if (LINK->Token != NULL && LINK->Token->kind == toknum)
		cmdgoto(LINK);
	else
		LINK->elseflag = TRUE;
}

//*****************************************************************************

void cmdelse(struct LOC_exec* LINK)
{
	LINK->Token = NULL;
}

//*****************************************************************************

BOOL skiploop(long up, long dn, struct LOC_exec* LINK)
{
	BOOL Result;
	long i;
	linerec* saveline;

	saveline = stmtline;
	i = 0;
	do
	{
		while (LINK->Token == NULL)
		{
			if (stmtline == NULL || stmtline->next == NULL)
			{
				Result = FALSE;
				stmtline = saveline;
				goto _L1;
			}
			stmtline = stmtline->next;
			LINK->Token = stmtline->txt;
		}
		if (LINK->Token->kind == up)
			i++;
		if (LINK->Token->kind == dn)
			i--;
		LINK->Token = LINK->Token->next;
	} while (i >= 0);
	Result = TRUE;
_L1:
	return Result;
}

//*****************************************************************************

void cmdfor(struct LOC_exec* LINK)
{
	looprec* l, lr;
	linerec* saveline;
	long i, j;

	lr.vp = findvar(LINK);
	if (lr.vp->IsStringVar)
		SyntaxError();
	RequireToken(tokeq, LINK);
	*lr.vp->AddressOfDoubleVar = DoubleExpression(LINK);
	RequireToken(tokto, LINK);
	lr.max = DoubleExpression(LINK);
	if (LINK->Token != NULL && LINK->Token->kind == tokstep)
	{
		LINK->Token = LINK->Token->next;
		lr.step = DoubleExpression(LINK);
	}
	else
		lr.step = 1.0;
	lr.homeline = stmtline;
	lr.hometok = LINK->Token;
	lr.kind = FORLOOP;
	lr.next = loopbase;
	//if (lr.step >= 0 && *lr.vp->AddressOfDoubleVar > lr.max || lr.step <= 0 && *lr.vp->AddressOfDoubleVar < lr.max)
	if ((lr.step >= 0 && *lr.vp->AddressOfDoubleVar > lr.max) || (lr.step <= 0 && *lr.vp->AddressOfDoubleVar < lr.max))
	{
		saveline = stmtline;
		i = 0;
		j = 0;
		do
		{
			while (LINK->Token == NULL)
			{
				if (stmtline == NULL || stmtline->next == NULL)
				{
					stmtline = saveline;
					Abort("FOR without NEXT");
				}
				stmtline = stmtline->next;
				LINK->Token = stmtline->txt;
			}
			if (LINK->Token->kind == tokfor)
			{
				if (LINK->Token->next != NULL && LINK->Token->next->kind == tokvar && LINK->Token->next->vp == lr.vp)
					j++;
				else
					i++;
			}
			if (LINK->Token->kind == toknext)
			{
				if (LINK->Token->next != NULL && LINK->Token->next->kind == tokvar && LINK->Token->next->vp == lr.vp)
					j--;
				else
					i--;
			}
			LINK->Token = LINK->Token->next;
		} while (i >= 0 && j >= 0);
		skiptoeos(LINK);
		return;
	}
	l = (looprec*)safe_calloc(sizeof(looprec), 1);
	*l = lr;
	loopbase = l;
}

//*****************************************************************************

void cmdnext(struct LOC_exec* LINK)
{
	varrec* v;
	BOOL found;
	looprec* l, * WITH;

	if (!IsEndOfStatement(LINK))
		v = findvar(LINK);
	else
		v = NULL;
	do
	{
		if (loopbase == NULL || loopbase->kind == GOSUBLOOP)
			Abort("NEXT without FOR");

		found = (loopbase->kind == FORLOOP && (v == NULL || loopbase->vp == v));

		if (!found)
		{
			l = loopbase->next;
			free(loopbase);
			loopbase = l;
		}
	} while (!found);

	WITH = loopbase;

	*WITH->vp->AddressOfDoubleVar += WITH->step;

	if ((WITH->step < 0 || *WITH->vp->AddressOfDoubleVar <= WITH->max) && (WITH->step > 0 || *WITH->vp->AddressOfDoubleVar >= WITH->max))
	{
		stmtline = WITH->homeline;
		LINK->Token = WITH->hometok;
		return;
	}
	l = loopbase->next;
	free(loopbase);
	loopbase = l;
}

void cmdwhile(struct LOC_exec* LINK)
{
	looprec* l;

	l = (looprec*)safe_calloc(sizeof(looprec), 1);
	l->next = loopbase;
	loopbase = l;
	l->kind = WHILELOOP;
	l->homeline = stmtline;
	l->hometok = LINK->Token;
	if (IsEndOfStatement(LINK))
		return;
	if (DoubleExpression(LINK) != 0)
		return;
	if (!skiploop(tokwhile, tokwend, LINK))
		Abort("WHILE without WEND");
	l = loopbase->next;
	free(loopbase);
	loopbase = l;
	skiptoeos(LINK);
}

void cmdwend(struct LOC_exec* LINK)
{
	tokenrec* tok;
	linerec* tokline;
	looprec* l;
	BOOL found;

	do
	{
		if (loopbase == NULL || loopbase->kind == GOSUBLOOP)
			Abort("WEND without WHILE");
		found = (loopbase->kind == WHILELOOP);
		if (!found)
		{
			l = loopbase->next;
			free(loopbase);
			loopbase = l;
		}
	} while (!found);
	if (!IsEndOfStatement(LINK))
	{
		if (DoubleExpression(LINK) != 0)
			found = FALSE;
	}
	tok = LINK->Token;
	tokline = stmtline;
	if (found)
	{
		stmtline = loopbase->homeline;
		LINK->Token = loopbase->hometok;
		if (!IsEndOfStatement(LINK))
		{
			if (DoubleExpression(LINK) == 0)
				found = FALSE;
		}
	}
	if (found)
		return;
	LINK->Token = tok;
	stmtline = tokline;
	l = loopbase->next;
	free(loopbase);
	loopbase = l;
}

//*****************************************************************

void cmddo(struct LOC_exec* LINK)
{
	looprec* l;

	l = (looprec*)safe_calloc(sizeof(looprec), 1);
	l->next = loopbase;
	loopbase = l;
	//*****************************************************************
	//    skip over WHILE keyword token ... added by Kevin Diggins
	//*****************************************************************

	if (!IsEndOfStatement(LINK))
		RequireToken(tokwhile, LINK);

	//*****************************************************************
	l->kind = WHILELOOP;
	l->homeline = stmtline;
	l->hometok = LINK->Token;

	if (IsEndOfStatement(LINK))
		return;
	if (DoubleExpression(LINK) != 0)
		return;
	if (!skiploop(tokwhile, tokloop, LINK))
		Abort("DO without LOOP");
	l = loopbase->next;
	free(loopbase);
	loopbase = l;
	skiptoeos(LINK);
}

//*****************************************************************

void cmdloop(struct LOC_exec* LINK)
{
	tokenrec* tok;
	linerec* tokline;
	looprec* l;
	BOOL found;

	do
	{
		if (loopbase == NULL || loopbase->kind == GOSUBLOOP)
			Abort("LOOP without DO");

		found = (loopbase->kind == WHILELOOP);

		if (!found)
		{
			l = loopbase->next;
			free(loopbase);
			loopbase = l;
		}
	} while (!found);

	//*****************************************************************************
	//        skip over UNTIL keyword token ... added by Kevin Diggins
	//*****************************************************************************

	if (!IsEndOfStatement(LINK))
		RequireToken(tokuntil, LINK);

	//*****************************************************************************
	if (!IsEndOfStatement(LINK))
	{
		if (DoubleExpression(LINK) != 0)
			found = FALSE;
	}
	//*****************************************************************************
	tok = LINK->Token;
	tokline = stmtline;

	if (found)

	{
		stmtline = loopbase->homeline;
		LINK->Token = loopbase->hometok;
		if (!IsEndOfStatement(LINK))
		{
			if (DoubleExpression(LINK) == 0)
				found = FALSE;
		}
	}

	if (found)
		return;

	LINK->Token = tok;
	stmtline = tokline;
	l = loopbase->next;
	free(loopbase);
	loopbase = l;
}

//*****************************************************************

void cmdgosub(struct LOC_exec* LINK)
{
	looprec* l;
	l = (looprec*)safe_calloc(sizeof(looprec), 1);
	l->next = loopbase;
	loopbase = l;
	l->kind = GOSUBLOOP;
	l->homeline = stmtline;
	l->hometok = LINK->Token;
	cmdgoto(LINK);
}

//*****************************************************************

void cmdreturn(struct LOC_exec* LINK)
{
	looprec* l;
	BOOL found;

	do
	{
		if (loopbase == NULL)
			Abort("RETURN without GOSUB");
		found = (loopbase->kind == GOSUBLOOP);
		if (!found)
		{
			l = loopbase->next;
			free(loopbase);
			loopbase = l;
		}
	} while (!found);
	stmtline = loopbase->homeline;
	LINK->Token = loopbase->hometok;
	l = loopbase->next;
	free(loopbase);
	loopbase = l;
	skiptoeos(LINK);
}

//*****************************************************************

void cmdread(struct LOC_exec* LINK)
{
	varrec* v;
	tokenrec* tok;
	BOOL found;

	do
	{
		v = findvar(LINK);
		tok = LINK->Token;
		LINK->Token = datatok;
		if (dataline == NULL)
		{
			dataline = linebase;
			LINK->Token = dataline->txt;
		}
		if (LINK->Token == NULL || LINK->Token->kind != tokcomma)
		{
			do
			{
				while (LINK->Token == NULL)
				{
					if (dataline == NULL || dataline->next == NULL)
						Abort("Out of Data");
					dataline = dataline->next;
					LINK->Token = dataline->txt;
				}
				found = (LINK->Token->kind == tokdata);
				LINK->Token = LINK->Token->next;
			} while (!found || IsEndOfStatement(LINK));
		}
		else
			LINK->Token = LINK->Token->next;
		if (v->IsStringVar)
		{
			if (*v->AddressOfStringVar != NULL)
				free(*v->AddressOfStringVar);
			*v->AddressOfStringVar = StringExpression(LINK);
		}
		else
			*v->AddressOfDoubleVar = DoubleExpression(LINK);
		datatok = LINK->Token;
		LINK->Token = tok;
		if (!IsEndOfStatement(LINK))
			RequireToken(tokcomma, LINK);
	} while (!IsEndOfStatement(LINK));
}

//*****************************************************************

void cmddata(struct LOC_exec* LINK)
{
	skiptoeos(LINK);
}

//*****************************************************************

void cmdrestore(struct LOC_exec* LINK)
{
	if (IsEndOfStatement(LINK))
		RestoreData();
	else
	{
		dataline = FindTargetLineNumber(IntegerExpression(LINK));
		datatok = dataline->txt;
	}
}

//****************************************************************

void cmdlocate(struct LOC_exec* LINK)
{
	int x, y;
	y = IntegerExpression(LINK);
	RequireToken(tokcomma, LINK);
	x = IntegerExpression(LINK);
	locate(y, x);
}

void cmdgotoxy(struct LOC_exec* LINK)
{
	int x, y;
	x = IntegerExpression(LINK);
	RequireToken(tokcomma, LINK);
	y = IntegerExpression(LINK);
	locate(y, x);
}
//****************************************************************

static void cmdmsgbox(struct LOC_exec* LINK)
{
	char* Tmp1;
	char* Tmp2;

	Tmp1 = (char*)safe_calloc(MAXSTRINGVAR, 1);
	Tmp2 = (char*)safe_calloc(MAXSTRINGVAR, 1);
	Tmp1 = StringExpression(LINK);
	RequireToken(tokcomma, LINK);
	Tmp2 = StringExpression(LINK);
	MessageBox(GetDesktopWindow(), Tmp1, Tmp2, 0);
	free(Tmp1);
	free(Tmp2);
}

//****************************************************************

static void cmdcolor(struct LOC_exec* LINK)
{
	long fg, bg;
	fg = IntegerExpression(LINK);
	RequireToken(tokcomma, LINK);
	bg = IntegerExpression(LINK);
	color(fg, bg);
}

//****************************************************************

static void cmdshell(struct LOC_exec* LINK)
{
	char* Tmp1;
	Tmp1 = (char*)safe_calloc(MAXSTRINGVAR, 1);
	Tmp1 = StringExpression(LINK);
	system(Tmp1);
	free(Tmp1);
}

//****************************************************************

static void cmdeval(struct LOC_exec* LINK)
{
	strcpy(G_inbuf, trim(StringExpression(LINK)));
	ParseInput(&tr_buf);
	stmttok = tr_buf; // Execute() uses a copy of buf, via stmttok
	if (stmttok != NULL)
		Execute();
	DisposeTokens(&tr_buf);
}

//****************************************************************

static void cmdkill(struct LOC_exec* LINK)
{
	char* Tmp1;
	Tmp1 = (char*)safe_calloc(MAXSTRINGVAR, 1);
	Tmp1 = StringExpression(LINK);
	DeleteFile(Tmp1);
	free(Tmp1);
}

//****************************************************************

void cmdon(struct LOC_exec* LINK)
{
	long i;
	looprec* l;

	i = IntegerExpression(LINK);
	if (LINK->Token != NULL && LINK->Token->kind == tokgosub)
	{
		l = (looprec*)safe_calloc(sizeof(looprec), 1);
		l->next = loopbase;
		loopbase = l;
		l->kind = GOSUBLOOP;
		l->homeline = stmtline;
		l->hometok = LINK->Token;
		LINK->Token = LINK->Token->next;
	}
	else

		RequireToken(tokgoto, LINK);

	if (i < 1)
	{
		skiptoeos(LINK);
		return;
	}

	while (i > 1 && !IsEndOfStatement(LINK))
	{
		RequireToken(toknum, LINK);
		if (!IsEndOfStatement(LINK))
			RequireToken(tokcomma, LINK);
		i--;
	}
	if (!IsEndOfStatement(LINK))
		cmdgoto(LINK);
}

//****************************************************************

void cmddim(struct LOC_exec* LINK)
{
	long i, j, k;
	varrec* v;
	BOOL done;

	do
	{
		if (LINK->Token == NULL || LINK->Token->kind != tokvar)
			SyntaxError();

		v = LINK->Token->vp;

		LINK->Token = LINK->Token->next;

		if (v->numdims != 0)
			Abort("Array already dimensioned");

		j = 1;
		i = 0;

		RequireToken(toklp, LINK);

		do
		{
			k = IntegerExpression(LINK) + 1;

			if (k < 1)
				BadSubScript();
			if (i >= MAXDIMS)
				BadSubScript();

			i++;
			v->dims[i - 1] = k;
			j *= k;

			done = (LINK->Token != NULL && LINK->Token->kind == tokrp);

			if (!done)
				RequireToken(tokcomma, LINK);
		} while (!done);

		LINK->Token = LINK->Token->next;
		v->numdims = i;

		if (v->IsStringVar)	// example: DIM A$(100) will create an array of NULL 32-bit pointers
		{
			v->sarr = (char**)safe_calloc(j, sizeof(char*));
			for (i = 0; i < j; i++)
				v->sarr[i] = NULL;
		}
		else
		{
			v->arr = (double*)safe_calloc(j, sizeof(double));
			for (i = 0; i < j; i++)
				v->arr[i] = 0.0;
		}
		if (!IsEndOfStatement(LINK))
			RequireToken(tokcomma, LINK);
	} while (!IsEndOfStatement(LINK));
}



//*****************************************************************************

void cmdfunction(struct LOC_exec* LINK)
{
	funcrec* f;
	varrec* v;

	// Allocate new function record
	f = (funcrec*)safe_calloc(1, sizeof(funcrec));
	f->next = funcbase;
	funcbase = f;

	// Get function name
	if (LINK->Token == NULL || LINK->Token->kind != tokvar)
		SyntaxError();

	v = LINK->Token->vp;
	strcpy(f->name, v->name);
	f->IsStringFunc = v->IsStringVar;
	LINK->Token = LINK->Token->next;

	// Parse parameters
	f->numparams = 0;
	if (LINK->Token != NULL && LINK->Token->kind == toklp) {
		LINK->Token = LINK->Token->next;  // Skip '('

		// Check if there are parameters (not immediately a closing paren)
		if (LINK->Token != NULL && LINK->Token->kind != tokrp) {
			while (LINK->Token != NULL && LINK->Token->kind != tokrp) {
				if (LINK->Token->kind != tokvar)
					SyntaxError();

				if (f->numparams >= MAXDIMS)
					Abort("Too many function parameters");

				strcpy(f->paramnames[f->numparams], LINK->Token->vp->name);
				f->numparams++;

				LINK->Token = LINK->Token->next;

				if (LINK->Token != NULL && LINK->Token->kind == tokcomma)
					LINK->Token = LINK->Token->next;
			}
		}

		// Require closing parenthesis
		RequireToken(tokrp, LINK);
	}

	// Expect = sign
	RequireToken(tokeq, LINK);

	// Store the rest of the line as the function body
	tokenrec* body_start = NULL;
	tokenrec* prev = NULL;

	while (LINK->Token != NULL && LINK->Token->kind != tokcolon) {
		tokenrec* t = (tokenrec*)safe_calloc(1, sizeof(tokenrec));

		// Copy token data
		t->kind = LINK->Token->kind;
		t->vp = LINK->Token->vp;
		t->num = LINK->Token->num;
		t->BadSyntaxChar = LINK->Token->BadSyntaxChar;
		t->next = NULL;

		// Deep copy strings
		if (t->kind == tokstr || t->kind == tokrem) {
			t->sp = (char*)safe_calloc(MAXSTRINGVAR, 1);
			strcpy(t->sp, LINK->Token->sp);
		}
		else {
			t->sp = NULL;
		}

		if (body_start == NULL) {
			body_start = t;
		}
		else {
			prev->next = t;
		}
		prev = t;

		LINK->Token = LINK->Token->next;
	}

	f->body = body_start;
}

//*****************************************************************************

funcrec* FindFunction(char* name)
{
	funcrec* f = funcbase;
	while (f != NULL && _stricmp(f->name, name))
		f = f->next;
	return f;
}

//*****************************************************************************

void ListFunctions(void)
{
	funcrec* f = funcbase;
	int i;

	if (f == NULL) {
		printf("No user-defined functions.\n");
		return;
	}

	printf("\n--- User-Defined Functions ---\n");

	while (f != NULL) {
		color(KWCOLOR, BGCOLOR);
		printf("FUNCTION ");
		color(FGCOLOR, BGCOLOR);
		printf("%s", mcase(f->name));

		if (f->numparams > 0) {
			printf("(");
			for (i = 0; i < f->numparams; i++) {
				printf("%s", mcase(f->paramnames[i]));
				if (i < f->numparams - 1)
					printf(", ");
			}
			printf(")");
		}

		printf(" = ");
		ListTokens(stdout, f->body);
		printf("\n");

		f = f->next;
	}
	printf("\n");
}






//****************************************************************

void Execute(void)
{
	struct LOC_exec V;
	char* ioerrmsg;
	char Str1[2048] = { 0 };
	Esc_Code = 0;
	//*********************************************
	TRY(try1);
	//*********************************************
	do
	{
		do
		{
			//*****************************************************************
			//  Stop a runaway program by pressing the ESCape key  (by MrBcx)
			//*****************************************************************

			if (_kbhit())
			{
				int asccodereturn = _getch();
				if (asccodereturn == 27)
				{
					Esc_Code = -20;
					goto _Ltry1;
				}
			}
			//****************************************************************
			V.gotoflag = FALSE;
			V.elseflag = FALSE;
			while (stmttok != NULL && stmttok->kind == tokcolon)
				stmttok = stmttok->next;
			V.Token = stmttok;

			if (V.Token != NULL)
			{
				V.Token = V.Token->next;
				switch (stmttok->kind)
				{
				case tokrem:
					/* blank case */
					break;

				case tokfunction:
					cmdfunction(&V);
					break;

				case tokokcancel:
					cmdokcancel(&V);
					break;

				case tokclear:
					ClearAll();
					break;

				case tokcls:
					cls();
					break;

				case toklist:
					cmdlist(&V);
					break;

				case toksleep:
					cmdsleep(&V);
					break;

				case toktextmode:
					cmdtextmode(&V);
					break;

				case tokrandomize:
					cmdrandomize(&V);
					break;

				case toksetcursor:
					cmdsetcursor(&V);
					break;

				case tokrun:
					cmdrun(&V);
					break;

				case toknew:
					cmdnew(&V);
					break;

				case tokload:
					cmdload(FALSE, StringExpressionEx(Str1, &V), &V);
					break;

				case tokmerge:
					cmdload(TRUE, StringExpressionEx(Str1, &V), &V);
					break;

				case toksave:
					cmdsave(&V);
					break;

				case tokbye:
					cmdbye();
					break;

				case tokdel:
					cmddel(&V);
					break;

				case tokrenum:
					cmdrenum(&V);
					break;

				case toklet:
					cmdlet(FALSE, &V);
					break;

				case tokvar:
					cmdlet(TRUE, &V);
					break;

				case tokprint:
					cmdprint(&V);
					break;

				case tokfprint:
					cmdfprint(&V);
					break;

				case tokflineinput:
					cmdflineinput(&V);
					break;

				case tokget:
					cmdget(&V);
					break;

				case tokput:
					cmdput(&V);
					break;

				case tokseek:
					cmdseek(&V);
					break;

				case tokclose:
					cmdclose(&V);
					break;

				case tokrewind:
					cmdrewind(&V);
					break;

				case tokinput:
					cmdinput(&V);
					break;

				case tokgoto:
					cmdgoto(&V);
					break;

				case tokif:
					cmdif(&V);
					break;

				case tokelse:
					cmdelse(&V);
					break;

				case tokend:
					cmdend(&V);
					break;

				case tokstop:
					Esc_Code = -20;
					goto _Ltry1;
					break;

				case tokfor:
					cmdfor(&V);
					break;

				case toknext:
					cmdnext(&V);
					break;

				case tokwhile:
					cmdwhile(&V);
					break;

				case tokwend:
					cmdwend(&V);
					break;

				case tokdo:
					cmddo(&V);
					break;

				case tokloop:
					cmdloop(&V);
					break;

				case tokgosub:
					cmdgosub(&V);
					break;

				case tokreturn:
					cmdreturn(&V);
					break;

				case tokread:
					cmdread(&V);
					break;

				case tokdata:
					cmddata(&V);
					break;

				case tokrestore:
					cmdrestore(&V);
					break;

				case toklocate:
					cmdlocate(&V);
					break;

				case tokgotoxy:
					cmdgotoxy(&V);
					break;

				case tokmsgbox:
					cmdmsgbox(&V);
					break;

				case tokcolor:
					cmdcolor(&V);
					break;

				case tokshell:
					cmdshell(&V);
					break;

				case tokeval:
					cmdeval(&V);
					break;

				case tokkill:
					cmdkill(&V);
					break;

				case tokopen:
					cmdopen(&V);
					break;

				case tokon:
					cmdon(&V);
					break;

				case tokswap:
					cmdswap(&V);
					break;

				case tokdim:
					cmddim(&V);
					break;

				default:
					Abort("Syntax error!");
					break;
				}
			}

			if (!V.elseflag && !IsEndOfStatement(&V))
				checkextra(&V);

			stmttok = V.Token;
		}

		while (V.Token != NULL);

		if (stmtline != NULL)
		{
			if (!V.gotoflag)
				stmtline = stmtline->next;
			if (stmtline != NULL)
				stmttok = stmtline->txt;
		}

	} while (stmtline != NULL);
	//*********************************************
	CATCH(try1, _Ltry1);
	//*********************************************
	if (Esc_Code == -20)
		printf("Break");

	else if (Esc_Code != 42)
	{
		switch (Esc_Code)
		{
		case -4:
			printf("Integer overflow");
			break;

		case -5:
			printf("Divide by zero");
			break;

		case -6:
			printf("Double math overflow");
			break;

		case -7:
			printf("Double math underflow");
			break;

		case -8:
		case -19:
		case -18:
		case -17:
		case -16:
		case -15:
			printf("Value range error");
			break;

		case -10:
			ioerrmsg = (char*)safe_calloc(MAXSTRINGVAR, 1);
			sprintf(ioerrmsg, "I/O Error %d", (int)io_Result);
			printf("%s", ioerrmsg);
			free(ioerrmsg);
			break;

		default:
			Critical_Escape(Esc_Code);
			break;
		}
	}
	if (stmtline != NULL)
		printf(" line number: %ld", stmtline->num);

	putchar('\n');
	//*********************************************
	END_TRY(try1);
	//*********************************************
}

//****************************************************************
//                     Begin BCX Library Functions
//****************************************************************

char* BCX_TmpStr(size_t Bites)
{
	static int StrCnt;
	static char* StrFunc[2048] = { 0 };

	StrCnt = (StrCnt + 1) & 2047;
	if (StrFunc[StrCnt])
		free(StrFunc[StrCnt]);
	return StrFunc[StrCnt] = (char*)safe_calloc(Bites + 1, sizeof(char));
}

//****************************************************************

double Round(double n, int d)
{
	return (floor((n)*pow(10.0, (d)) + 0.5) / pow(10.0, (d)));
}

//****************************************************************

char *Using(char * szMask, double Num)
{
  int CntDec = 0;    // Count the number of "0" to left pad the result
  int Decimals = 0;  // Count ALL digits RIGHT of the decimal point
  int Dollar = 0;    // Flag to LEFT prepend "$" symbol
  int Percent = 0;   // Flag to convert result to percent
  int SN = 0;        // Flag to use Sci Notation 
  int SN_Digits = 0; // Count of digits RIGHT of decimal
  int Spaces = 0;    // Flag for using spaces padding
  int Zeros = 0;     // Flag for using zero padding 
 
  char *BCX_RetStr = BCX_TmpStr(256);
  char Buf_1[256]={0};
  LPCTSTR p = szMask;
  int len;
 
  // First pass: analyze the format mask
 
  while (*p)
  {
    if (*p == 36)  Dollar++;      // $
    if (Dollar>1)  Dollar=1;      // Restrict
    if (*p == 37)  Percent++;     // %
    if (Percent>1) Percent=1;     // Restrict
    if (*p == 32)  Spaces++;      // Space
 
    // Track zero padding before decimal
    if (*p == 48 && !CntDec)  Zeros++;
 
    // Count decimal places (spaces, # or 0 after decimal point)
    if ((*p == 32 || *p == 35 || *p == 48) && CntDec)  Decimals++;
 
    // Count # symbols after decimal for SN precision
    if (*p == 35 && SN && CntDec)  SN_Digits++;
 
    if (*p == 46)  CntDec = 1;    // Decimal point
 
    if (*p == 94)  SN++;          // ^ is for scientific notation
 
    p++;
  }
 
  // Second pass: Assemble the resulting string
 
  if (SN)                         // Process SN and return
  {
    char Tmp[64]={0};
    char fmt[16]={0};
    
    // Use SN_Digits for precision after decimal, add 1 for the digit before decimal
    int precision = SN_Digits;
    sprintf(fmt, "%%.%dE", precision);
    sprintf(Tmp, fmt, (double)Num);
    
    // Clean up scientific notation
    while (instr(Tmp, "0E"))
      strcpy(Tmp, iReplace(Tmp, "0E", "E"));
    while (instr(Tmp, "E+00"))
      strcpy(Tmp, iReplace(Tmp, "E+00", "E+0"));
    while (instr(Tmp, "E-00"))
      strcpy(Tmp, iReplace(Tmp, "E-00", "E-0"));
    strcpy(BCX_RetStr, Tmp);
    return BCX_RetStr;
  }
 
  if(Percent) Num = Num * 100.0;
  // Format the number with specified decimals
  sprintf(Buf_1, "%1.*f", Decimals, Num);
 
  // Find integer part length
  char *decimal_pos = strchr(Buf_1, '.');
  int num_int_digits = decimal_pos ? (int)(decimal_pos - Buf_1) : (int)strlen(Buf_1);
 
  // Adjust for negative sign
  int has_negative = (Buf_1[0] == '-');
  if (has_negative)
    num_int_digits--;
 
  // Initialize the return string
  memset(BCX_RetStr, 0, 256);
 
  int zero_pad = 0;
 
  // Calculate zero padding (apply only if needed)
  if (Zeros > 0 && Zeros > num_int_digits)
    zero_pad = Zeros - num_int_digits;
 
  // Position for the start of the number
  int pos = Dollar + Spaces + zero_pad;
 
  // Copy the number to the return string
  if (has_negative)
  {
    if (Zeros > 0)
    {
      // With zero padding, negative goes before zeros
      BCX_RetStr[Dollar + Spaces] = '-';
      strcpy(BCX_RetStr + pos, Buf_1 + 1);
    }
    else
    {
      // Without zero padding, negative stays with number
      strcpy(BCX_RetStr + pos, Buf_1);
    }
  }
  else
  {
    strcpy(BCX_RetStr + pos, Buf_1);
  }
 
  // Add commas for thousands
  char temp[256]={0};
  strcpy(temp, BCX_RetStr);
 
  // Find where the number starts after any potential padding
  int number_start = Dollar + Spaces + zero_pad;
 
  // Handle negative sign position for proper comma placement
  if (has_negative && Zeros == 0)
    number_start++; // Skip past the negative sign for comma placement
 
  // Find integer part end
  decimal_pos = strchr(temp, '.');
  int int_end = decimal_pos ? (int)(decimal_pos - temp) : (int)strlen(temp);
 
  // Add commas working backwards, but never insert a comma right after the negative sign
  for (int i = int_end - 3; i > number_start; i -= 3)
  {
    // Ensure we don't place a comma immediately after a negative sign
    if (i > number_start && !(temp[i-1] == '-'))
    {
      memmove(temp + i + 1, temp + i, (int)strlen(temp + i) + 1);
      temp[i] = ',';
    }
  }
 
  strcpy(BCX_RetStr, temp);
 
  // Add dollar sign
  if (Dollar)
  {
    if (Num >= 0)
    {
      BCX_RetStr[Spaces] = '$';
    }
    else
    {
      if (Zeros > 0)
      {
        // With zero padding: move negative, then add dollar
        BCX_RetStr[Spaces] = '-';
        BCX_RetStr[Spaces + 1] = '$';
        
        // Remove duplicate negative sign
        for (int i = pos; i < (int)strlen(BCX_RetStr); i++)
        {
          if (BCX_RetStr[i] == '-')
          {
            memmove(BCX_RetStr + i, BCX_RetStr + i + 1, (int)strlen(BCX_RetStr + i));
            break;
          }
        }
      }
      else
      {
        // Without zero padding: dollar then negative
        BCX_RetStr[Spaces] = '$';
      }
    }
  }
 
  // Fill leading spaces
  if (Spaces)
    memset(BCX_RetStr, ' ', Spaces);
 
  // Fill zeros after spaces but only if needed
  for (int i = 0; i < zero_pad; i++)
    BCX_RetStr[Dollar + Spaces + i] = '0';
 
  // Add percent sign if needed
  if (Percent)
  {
    len = (int)strlen(BCX_RetStr);
    BCX_RetStr[len] = '%';
    BCX_RetStr[len + 1] = 0;
  }
  return BCX_RetStr;
}

//****************************************************************

char* str(double d)
{
	char* strtmp = BCX_TmpStr(24);
	sprintf(strtmp, "% .15G", d);
	return strtmp;
}
//****************************************************************

char* ucase(char* S)
{
	char* strtmp = BCX_TmpStr((int)strlen(S));
	return _strupr(strcpy(strtmp, S));
}

//****************************************************************

char* lcase(char* S)
{
	char* strtmp = BCX_TmpStr((int)strlen(S));
	return _strlwr(strcpy(strtmp, S));
}

//****************************************************************

char* mcase(char* S)
{
	char* strtmp = BCX_TmpStr((int)strlen(S) + 1);
	char* s = strtmp;
	strcpy(strtmp, S);
	_strlwr(strtmp);
	while (*s)
	{
		if (islower(*s))
		{
			*s -= 32;
			while (isalpha(*++s));
		}
		s++;
	}
	return strtmp;
}

//****************************************************************

char* left(char* S, int length)
{
	int tmplen = (int)strlen(S);
	char* strtmp = BCX_TmpStr(tmplen);

	strcpy(strtmp, S);
	if (length > tmplen)
		strtmp[tmplen] = 0;
	else
		strtmp[length] = 0;
	return strtmp;
}

//****************************************************************

char* right(char* S, int length)
{
	int tmplen = (int)strlen(S);
	char* BCX_RetStr = BCX_TmpStr(tmplen);

	tmplen -= length;
	if (tmplen < 0)
		tmplen = 0;
	return strcpy(BCX_RetStr, S + tmplen);
}

//****************************************************************

char* extract(char* mane, char* match)
{
	char* a;
	char* strtmp = BCX_TmpStr((int)strlen(mane));

	strcpy(strtmp, mane);
	a = strstr(mane, match);
	if (a)
		strtmp[a - mane] = 0;
	return strtmp;
}

//****************************************************************

char* remain(char* mane, char* mat)
{
	char* p = strstr(mane, mat);

	if (p)
	{
		p += ((int)strlen(mat));
		return p;
	}
	return mane;
}

//****************************************************************

char* RemoveStr(char* a, char* b)
{
	int i, tmplen = (int)strlen(b);
	char* strtmp = BCX_TmpStr(strlen(a));

	strcpy(strtmp, a);
	while (1)
	{
		i = instr(strtmp, b);
		if (i == 0)
			break;
		strcpy(&strtmp[i - 1], &strtmp[i + tmplen - 1]);
	}
	return strtmp;
}

//****************************************************************

char* iReplace(char* src, char* pat, char* rep)
{
	size_t patsz, repsz, tmpsz, delta;
	char* strtmp, * p, * q, * r;


	if (!pat || !*pat)
	{
		strtmp = BCX_TmpStr(strlen(src));
		if (!strtmp)
			return NULL;
		return strcpy(strtmp, src);
	}

	repsz = strlen(rep);
	patsz = strlen(pat);
	for (tmpsz = 0, p = src; (q = _stristr_(p, pat)) != 0; p = q + patsz)
		tmpsz += (size_t)(q - p) + repsz;
	tmpsz += strlen(p);
	strtmp = BCX_TmpStr(tmpsz);
	if (!strtmp)
		return NULL;
	for (r = strtmp, p = src; (q = _stristr_(p, pat)) != 0; p = q + patsz)
	{
		delta = (size_t)(q - p);
		strncpy(r, p, delta);
		r += delta;
		strcpy(r, rep);
		r += repsz;
	}
	strcpy(r, p);
	return strtmp;
}

//****************************************************************

char* trim(char* S)
{
	int i = (int)strlen(S);
	char* strtmp = BCX_TmpStr(i);
	strcpy(strtmp, S);
	while (i--)
	{
		if (S[i] != 0 && S[i] != 9 && S[i] != 10 && S[i] != 11 && S[i] != 13 && S[i] != 32)
			break;
	}
	strtmp[++i] = 0;
	while (*strtmp == '\x09' || *strtmp == '\x0A' || *strtmp == '\x0B' || *strtmp == '\x0D' || *strtmp == '\x20')
		strtmp++;
	return strtmp;
}

//****************************************************************

char* enc(char* A)
{
	char* BCX_RetStr = BCX_TmpStr(strlen(A) + 3);
	int L = 34;
	int R = 34;
	sprintf(BCX_RetStr, "%c%s%c%s", L, A, R, "");
	return BCX_RetStr;
}

//****************************************************************

char* ltrim(char* S)
{
	if (S[0] == 0)
		return S;
	char* strtmp = BCX_TmpStr(strlen(S));

	strcpy(strtmp, S);
	while (*strtmp == 32)
		strtmp++;
	return strtmp;
}

//****************************************************************

char* rtrim(char* S, char c)
{
	if (S[0] == 0)
		return S;
	int i = (int)strlen(S);
	while (i > 0 && (S[i - 1] == c || S[i - 1] == 32))
		i--;
	char* strtmp = BCX_TmpStr(i);
	return (char*)memcpy(strtmp, S, i);
}

//****************************************************************

char* timef(void)
{
	time_t elapse_time;
	struct tm* tp;
	char A[256] = { 0 };
	char* strtmp = BCX_TmpStr(256);
	time(&elapse_time);
	tp = localtime(&elapse_time);
	A[0] = 0;
	strftime(A, 256, "%H:%M:%S", tp);
	strcpy(strtmp, A);
	return strtmp;
}

//****************************************************************

int instr(char* mane, char* match, int offset, int sensflag)
{
	char* s;

	if (!mane || !match || !*match || offset > (int)strlen(mane))
		return 0;
	if (sensflag)
		s = _stristr_(offset > 0 ? mane + offset - 1 : mane, match);
	else
		s = strstr(offset > 0 ? mane + offset - 1 : mane, match);
	return s ? (int)(s - mane) + 1 : 0;
}

//****************************************************************

char* _stristr_(char* String, char* Pattern)
{
	char* pptr, * sptr, * start;
	UINT slen, plen;

	for (start = (char*)String, pptr = (char*)Pattern, slen = (UINT)strlen(String), plen = (UINT)strlen(Pattern); slen >= plen; start++, slen--)
	{
		while (toupper(*start) != toupper(*Pattern))
		{
			start++;
			slen--;
			if (slen < plen)
				return (0);
		}
		sptr = start;
		pptr = (char*)Pattern;
		while (toupper(*sptr) == toupper(*pptr))
		{
			sptr++;
			pptr++;
			if (!*pptr)
				return (start);
		}
	}
	return (0);
}

//****************************************************************

void locate(int row, int col, int show, int shape)
{
	CONSOLE_CURSOR_INFO cci = { 0 };

	cursor.X = (SHORT)(col - 1);
	cursor.Y = (SHORT)(row - 1);
	SetConsoleCursorPosition(hConsole, cursor);
	cci.bVisible = show;
	cci.dwSize = shape;
	SetConsoleCursorInfo(hConsole, &cci);
}

//****************************************************************

void color(long fg, long bg)
{
	SetConsoleTextAttribute(hConsole, (WORD)fg + (WORD)bg * 16);
	color_fg = fg;
	color_bg = bg;
}

//****************************************************************

void cls(void)
{
	COORD coordScreen = { 0, 0 };
	DWORD ccharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
	DWORD ConSize;
	WORD attr;

	cursor.X = 0;
	cursor.Y = 0;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	ConSize = csbi.dwSize.X * csbi.dwSize.Y;
	attr = (WORD)(color_fg + color_bg * 16);
	FillConsoleOutputAttribute(hConsole, attr, ConSize, coordScreen, &ccharsWritten);
	FillConsoleOutputCharacter(hConsole, 32, ConSize, coordScreen, &ccharsWritten);
	locate(1, 1, 1);
}

//****************************************************************

int like(char* raw, char* pat)
{
	char a = 0, b = 0, d = 0;
	char* r, * p;
	int star = 0;
	while (1)
	{
		if ((d = *pat++) == 0)
		{
			return (star || !*raw);
		}
		else if (d == '*')
		{
			star = 1;
		}
		else if (d == '?')
		{
			if (!*raw++)
			{
				return 0;
			}
		}
		else
		{
			break;
		}
	}
	b = d;
	do
	{
		if ((a = *raw++) == b)
		{
			r = raw;
			p = pat;
			do
			{
				if ((d = *p++) == '*')
				{
					if (like(r, p - 1))
					{
						return 1;
					}
					else
					{
						break;
					}
				}
				else if (!d)
				{
					if (!*r)
					{
						return 1;
					}
					else
					{
						break;
					}
				}
			} while (*r++ == d || d == '?');
		}
	} while (star && a);
	return 0;
}


//****************************************************************

char* GetFileName(char* Title, char* Filter, int Flag, HWND hWnd, DWORD Flags, char* InitialDir, char* Initfname, int* ExtIdx)
{
	OPENFILENAME OpenFileStruct;
	char Extension[256] = { 0 };
	UINT Counter = 0, TmpSize;
	static char* filename;
	static UINT BufSize;

	TmpSize = ((Flags & OFN_ALLOWMULTISELECT) ? 5000 : MAX_PATH);

	if (TmpSize > BufSize)
	{
		BufSize = TmpSize;
		filename = (char*)realloc(filename, BufSize);
	}
	memset(filename, 0, BufSize);
	memset(Extension, 0, 256);
	memset(&OpenFileStruct, 0, sizeof(OpenFileStruct));

	if (Initfname)
		strcpy(filename, Initfname);

	for (Counter = 0; Counter <= (UINT)strlen(Filter); Counter++)
	{
		if (Filter[Counter] == '|')
			Extension[Counter] = 0;
		else
			Extension[Counter] = Filter[Counter];
	}
	CmDlgHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)SBProc, (HINSTANCE)NULL, GetCurrentThreadId());
	OpenFileStruct.lStructSize = sizeof(OpenFileStruct);
	OpenFileStruct.hwndOwner = hWnd;
	OpenFileStruct.hInstance = 0;
	OpenFileStruct.lpstrFilter = Extension;
	OpenFileStruct.lpstrTitle = Title;
	OpenFileStruct.nMaxFile = BufSize;
	OpenFileStruct.nMaxFileTitle = 0;
	OpenFileStruct.lpstrFile = filename;
	OpenFileStruct.lpstrFileTitle = NULL;
	OpenFileStruct.lpstrCustomFilter = 0;
	OpenFileStruct.nMaxCustFilter = 0;
	if (ExtIdx)
		OpenFileStruct.nFilterIndex = *ExtIdx;
	OpenFileStruct.lpstrInitialDir = InitialDir;
	OpenFileStruct.nFileOffset = 0;
	OpenFileStruct.nFileExtension = 0;
	OpenFileStruct.lpstrDefExt = 0;
	OpenFileStruct.lCustData = 0;
	OpenFileStruct.lpfnHook = 0;
	OpenFileStruct.lpTemplateName = 0;
	if (!Flags)
		OpenFileStruct.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_EXPLORER;
	else
		OpenFileStruct.Flags = Flags | OFN_EXPLORER;
	if (!Flag)
	{
		if (GetOpenFileName(&OpenFileStruct) == 0)
		{
			*filename = 0;
		}
		else
		{
			int len = (int)strlen(filename);
			if (filename[len + 1] == 0)
				return filename;

			char* fname = filename + len;
			while (fname[1])
			{
				*fname = ',';
				len = (UINT)strlen(++fname);
				fname += len;
			}
		}
	}
	else
	{
		if (GetSaveFileName(&OpenFileStruct) == 0)
			*filename = 0;
	}
	if (ExtIdx)
		*ExtIdx = OpenFileStruct.nFilterIndex;
	return filename;
}

//****************************************************************

LRESULT CALLBACK SBProc(int Msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	if (Msg == HCBT_ACTIVATE)
	{
		static RECT rc1;
		static RECT rc2;
		GetWindowRect(GetDesktopWindow(), &rc1);
		GetWindowRect((HWND)wParam, &rc2);
		SetWindowPos((HWND)wParam, HWND_TOP, (rc1.left + rc1.right - rc2.right + rc2.left) / 2, (rc1.top + rc1.bottom - rc2.bottom + rc2.top) / 2, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		UnhookWindowsHookEx(CmDlgHook);
	}
	return 0;
}

//****************************************************************

void OK_CANCEL(char* Title, char* Message)
{
	MessageBox(GetActiveWindow(), Message, Title, 1);
}

//****************************************************************

int YN_CANCEL(char* Title, char* Message)
{
	int a = 0;
	int b = 0;

	a = MessageBox(GetActiveWindow(), Message, Title, 3);
	for (;;)
	{
		if (a == 2)
		{
			b = 3;
			break;
		}
		if (a == 6)
		{
			b = 1;
			break;
		}
		if (a == 7)
		{
			b = 2;
		}
		break;
	}
	return b;
}

//****************************************************************

char* InputBox(char* Title, char* Prompt, char* Value)
{
	char* BCX_RetStr;
	CreatePrompter(Title, Prompt, Value);
	BCX_RetStr = BCX_TmpStr(strlen(BCX_INPUTBOX_VAL));
	return strcpy(BCX_RetStr, BCX_INPUTBOX_VAL);
}

//****************************************************************

LRESULT CreatePrompter(char* Title, char* Prpt, char* Value)
{
	LPDLGITEMTEMPLATE lpdit;
	LPDLGTEMPLATE lpdt;
	HINSTANCE hInst;
	HGLOBAL hgbl;
	LRESULT ret;
	LPWORD lpw;
	LPWSTR lpwsz;
	DWORD MyStyle;
	int ID_CANCEL = 101;
	int ID_TEXT = 102;
	int ID_EDIT = 103;
	int ID_OK = 104;

	hgbl = GlobalAlloc(GMEM_ZEROINIT, MAXSTRINGVAR);
	lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

	MyStyle = WS_VISIBLE | DS_NOFAILCREATE | WS_BORDER | DS_CENTER | DS_SETFONT;

	lpdt->style = MyStyle;
	lpdt->cdit = 4;
	lpdt->cx = 150;
	lpdt->cy = 60;
	lpw = (LPWORD)(lpdt + 1);
	*lpw++ = 0;
	*lpw++ = 0;
	lpwsz = (LPWSTR)lpw;
	lpw += (WORD)MultiByteToWideChar(CP_ACP, 0, Title, -1, lpwsz, (int)(1 + strlen(Title) * 2));
	*lpw++ = DEFAULT_PITCH;
	lpwsz = (LPWSTR)lpw;
	lpw += (WORD)MultiByteToWideChar(CP_ACP, 0, "MS Sans Serif", -1, lpwsz, 27);

	// CANCEL button
	lpw = lpwAlign(lpw);
	lpdit = (LPDLGITEMTEMPLATE)lpw;
	lpdit->x = 95;
	lpdit->y = 31;
	lpdit->cx = 35;
	lpdit->cy = 12;
	lpdit->id = (WORD)ID_CANCEL;
	lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP;
	lpw = (LPWORD)(lpdit + 1);
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0080;
	lpwsz = (LPWSTR)lpw;
	lpw += (WORD)MultiByteToWideChar(CP_ACP, 0, "CANCEL", -1, lpwsz, 13);
	*lpw++ = 0;

	// OK button
	lpw = lpwAlign(lpw);
	lpdit = (LPDLGITEMTEMPLATE)lpw;
	lpdit->x = 20;
	lpdit->y = 31;
	lpdit->cx = 35;
	lpdit->cy = 12;
	lpdit->id = (WORD)ID_OK;
	lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP;
	lpw = (LPWORD)(lpdit + 1);
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0080;
	lpwsz = (LPWSTR)lpw;
	lpw += (WORD)MultiByteToWideChar(CP_ACP, 0, "OK", -1, lpwsz, 5);
	*lpw++ = 0;

	// STATIC control
	lpw = lpwAlign(lpw);
	lpdit = (LPDLGITEMTEMPLATE)lpw;
	lpdit->x = 10;
	lpdit->y = 5;
	lpdit->cx = 140;
	lpdit->cy = 10;
	lpdit->id = (WORD)ID_TEXT;
	lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;
	lpw = (LPWORD)(lpdit + 1);
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0082;
	lpwsz = (LPWSTR)lpw;
	lpw += (WORD)MultiByteToWideChar(CP_ACP, 0, Prpt, -1, lpwsz, (int)(1 + strlen(Prpt) * 2));
	*lpw++ = 0;

	// EDIT control
	lpw = (LPWORD)lpwAlign(lpw);
	lpdit = (LPDLGITEMTEMPLATE)lpw;
	lpdit->x = 10;
	lpdit->y = 15;
	lpdit->cx = 130;
	lpdit->cy = 12;
	lpdit->id = (WORD)ID_EDIT;
	lpdit->style = WS_CHILD | WS_TABSTOP | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER;
	lpw = (LPWORD)(lpdit + 1);
	*lpw++ = 0xFFFF;
	*lpw++ = 0x0081;
	lpwsz = (LPWSTR)lpw;
	lpw += (WORD)MultiByteToWideChar(CP_ACP, 0, Value, -1, lpwsz, (int)(1 + strlen(Value) * 2));
	*lpw++ = 0;

	GlobalUnlock(hgbl);
	hInst = (HINSTANCE)(LONG_PTR)GetWindowLongPtr(0, GWLP_HINSTANCE);
	ret = DialogBoxIndirect(hInst, (LPDLGTEMPLATE)hgbl, GetActiveWindow(), (DLGPROC)Prompter);
	GlobalFree(hgbl);
	return ret;
}

//****************************************************************

LRESULT CALLBACK Prompter(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	int ID_CANCEL = 101;
	int ID_EDIT = 103;
	int ID_OK = 104;

	for (;;)
	{
		if (Msg == WM_INITDIALOG)
		{
			SetFocus(GetDlgItem(hWnd, ID_EDIT));
			return 0;
		}
		if (Msg == WM_COMMAND)
		{
			if (LOWORD(wParam) == ID_OK)
			{
				GetDlgItemText(hWnd, ID_EDIT, BCX_INPUTBOX_VAL, 2047);
				EndDialog(hWnd, 0);
			}
			if (LOWORD(wParam) == ID_CANCEL)
			{
				*BCX_INPUTBOX_VAL = 0;
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;
		}
		if (Msg == WM_CLOSE)
			EndDialog(hWnd, 0);
		break;
	}
	return 0;
}

//****************************************************************

LPWORD lpwAlign(LPWORD lpIn)
{
	BYTE* ul;
	ul = (BYTE*)lpIn;
	while ((ULONGLONG)ul & 3)
	{
		ul++;
	}
	return (LPWORD)ul;
}

//****************************************************************

DWORD lof(char* FileName)
{
	WIN32_FIND_DATA W32FD;
	HANDLE hFile;
	int FSize;
	if (strlen(FileName) == 0)
		return 0;
	hFile = FindFirstFile(FileName, &W32FD);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FSize = W32FD.nFileSizeLow;
		FindClose(hFile);
		return FSize;
	}
	return 0;
}

//****************************************************************

char* curdir(void)
{
	char* strtmp = BCX_TmpStr(2048);
	GetCurrentDirectory(1024, strtmp);
	return strtmp;
}

//****************************************************************

char* tempdir(void)
{
	char* strtmp = BCX_TmpStr(2048);
	GetTempPath(1024, strtmp);
	return strtmp;
}

//****************************************************************

char* sysdir(void)
{
	char* strtmp = BCX_TmpStr(2048);
	GetSystemDirectory(strtmp, 2048);
	return strtmp;
}

//****************************************************************

char* windir(void)
{
	char* strtmp = BCX_TmpStr(2048);
	GetWindowsDirectory(strtmp, 2048);
	return strtmp;
}

//****************************************************************

BOOL Exist(char* szFilePath)
{
	if (instr(szFilePath, "*") || instr(szFilePath, "?"))
		return Exist_A(szFilePath);
	return Exist_B(szFilePath);
}

BOOL Exist_A(char* szFilePath)
{
	WIN32_FIND_DATA W32FindData;
	HANDLE rc;
	rc = FindFirstFile(szFilePath, &W32FindData);
	if (rc == INVALID_HANDLE_VALUE)
		return FALSE;
	FindClose(rc);
	return TRUE;
}

BOOL Exist_B(char* szFilePath)
{
	DWORD ret;
	ret = GetFileAttributes(szFilePath);
	if (ret != 0xffffffff)
		return TRUE;
	return FALSE;
}

//****************************************************************

char* findfirst(char* S)
{
	char* strtmp = BCX_TmpStr((int)strlen(S));
	if (FileHandle)
		FindClose(FileHandle);
	FileHandle = FindFirstFile(S, &FindData);
	if (FileHandle == INVALID_HANDLE_VALUE)
		*strtmp = 0;
	else
		strcpy(strtmp, FindData.cFileName);
	return strtmp;
}

//****************************************************************

char* findnext(void)
{
	char* strtmp = BCX_TmpStr(2048);
	int Found = FindNextFile(FileHandle, &FindData);
	if (Found > 0)
		strcpy(strtmp, FindData.cFileName);
	return strtmp;
}
//****************************************************************

void Center(HWND hwnd)
{
	RECT DesktopArea;
	RECT rc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &DesktopArea, 0);
	GetWindowRect(hwnd, &rc);
	SetWindowPos(hwnd, HWND_TOP, ((DesktopArea.right - DesktopArea.left) - (rc.right - rc.left)) / 2 + DesktopArea.left, ((DesktopArea.bottom - DesktopArea.top) - (rc.bottom - rc.top)) / 2 + DesktopArea.top, 0, 0, SWP_NOSIZE);
	return;
}


//****************************************************************

char* join(int n, ...)
{
	int i = n, tmplen = 0;
	char* s_;
	char* strtmp;
	va_list marker;
	va_start(marker, n);
	while (i-- > 0)
	{
		s_ = va_arg(marker, char*);
		if (s_)
			tmplen += (int)strlen(s_);
	}
	strtmp = BCX_TmpStr(tmplen);
	va_end(marker);
	i = n;
	va_start(marker, n);
	while (i-- > 0)
	{
		s_ = va_arg(marker, char*);
		if (s_)
			strcat(strtmp, s_);
	}
	va_end(marker);
	return strtmp;
}

//****************************************************************

char* inkey(void)
{
	char* strtmp = BCX_TmpStr(2);
	if (_kbhit())
	{
		int asccodereturn = _getch();
		strtmp[0] = (char)asccodereturn;
		strtmp[1] = 0;
	}
	return strtmp;
}
//****************************************************************
//                     End BCX Library Functions
//****************************************************************

//****************************************************************
//               MrBcx's OPEN FILE helper Functions
//****************************************************************

FILE* GetFilePtr(int FP)
{
	{
		int i;
		for (i = 1; i <= 255; i += 1)
		{
			if (FILEREC[i].FileNumber == FP)
			{
				return FILEREC[i].FilePtr;
			}
		}
	}
	return NULL;
}

//****************************************************************

void SetFilePtr(int FileNumber, FILE* FP)
{
	{
		int i;
		for (i = 1; i <= 255; i += 1)
		{
			if (FILEREC[i].FileNumber == 0)
			{
				FILEREC[i].FileNumber = FileNumber;
				FILEREC[i].FilePtr = FP;
				return;
			}
		}
	}
}

//****************************************************************

void UnSetFilePtr(int FileNumber)
{
	{
		int i;
		for (i = 1; i <= 255; i += 1)
		{
			if (FILEREC[i].FileNumber == FileNumber)
			{
				FILEREC[i].FileNumber = 0;
				FILEREC[i].FilePtr = NULL;
				return;
			}
		}
	}
}

//****************************************************************

void OpenFileForReading(int FileNumber, char* FileName)
{
	static FILE* A;
	A = NULL;

	A = fopen(FileName, "r");

	if (A)
	{
		SetFilePtr(FileNumber, A);
		return;
	}
	else
	{
		printf("%s\n", "Cannot open file for reading");
	}
}

//****************************************************************

void OpenFileForWriting(int FileNumber, char* FileName)
{
	static FILE* A;
	A = NULL;
	A = fopen(FileName, "w");
	if (A)
	{
		SetFilePtr(FileNumber, A);
		return;
	}
	else
	{
		printf("%s\n", "Cannot open file for writing");
	}
}

//****************************************************************

void OpenFileForAppending(int FileNumber, char* FileName)
{
	static FILE* A;
	A = NULL;

	A = fopen(FileName, "a");
	if (A)
	{
		SetFilePtr(FileNumber, A);
		return;
	}
	else
	{
		printf("%s\n", "Cannot open file for appending");
	}
}

//****************************************************************

void OpenFileForBinary(int FileNumber, char* FileName)
{
	static FILE* A;
	A = NULL;

	if (Exist(FileName))
	{
		A = fopen(FileName, "rb+");
		if (A)
		{
			SetFilePtr(FileNumber, A);
			return;
		}
		else
		{
			printf("%s\n", "Cannot open file for binary");
		}
	}
	else
	{
		A = fopen(FileName, "wb+");
		if (A)
		{
			SetFilePtr(FileNumber, A);
			return;
		}
		else
		{
			printf("%s\n", "Cannot open file for binary");
		}
	}
}

//****************************************************************

int EoF(FILE* stream)
{
	if (stream == NULL)
		Abort("File Not Open");
	int c, status = ((c = fgetc(stream)) == EOF);
	ungetc(c, stream);
	return status;
}

//****************************************************************

char* space(int count)
{
	if (count < 1)
		return BCX_TmpStr(1);
	char* strtmp = BCX_TmpStr(count);
	return (char*)memset(strtmp, 32, count);
}

//****************************************************************

void BumpDown(void)
{
	Indent--;
	Indent--;
	if (Indent < 0)
		Indent = 0;
	strcpy(Scoot, space(Indent));
}

//****************************************************************

void BumpUp(void)
{
	if (Indent < 0)
		Indent = 0;
	Indent++;
	Indent++;
	strcpy(Scoot, space(Indent));
}

//****************************************************************

int keypress(void)
{
	char uchr[] =
	{
	  0x7E, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26, 0x2A, 0x28, 0x29,
	  0x5F, 0x2B, 0x7C, 0x7B, 0x7D, 0x3A, 0x22, 0x3C, 0x3E, 0x3F, 0x60,
	  0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2D,
	  0x3D, 0x5C, 0x5B, 0x5D, 0x3B, 0x27, 0x2C, 0x2E, 0x2F, 0x00
	};

	char lchr[] =
	{
	  0x60, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
	  0x2D, 0x3D, 0x5C, 0x5B, 0x5D, 0x3B, 0x27, 0x2C, 0x2E, 0x2F, 0x7E,
	  0x21, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26, 0x2A, 0x28, 0x29, 0x5F,
	  0x2B, 0x7C, 0x7B, 0x7D, 0x3A, 0x22, 0x3C, 0x3E, 0x3F, 0x00
	};

	int ch = 0;
	INPUT_RECORD InputRecord;
	DWORD Count = 0, cks;
	WORD vkc, vsc;
	HANDLE hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
	PeekConsoleInput(hConsoleIn, &InputRecord, 1, &Count);
	DWORD OldConsoleMode;
	GetConsoleMode(hConsoleIn, &OldConsoleMode);
	SetConsoleMode(hConsoleIn, 0);
	UINT i = 0;
	do
	{
		ReadConsoleInput(hConsoleIn, &InputRecord, 1, &Count);
		SetConsoleMode(hConsoleIn, OldConsoleMode);
		if (Count && InputRecord.EventType == KEY_EVENT && InputRecord.Event.KeyEvent.bKeyDown)
		{
			vkc = InputRecord.Event.KeyEvent.wVirtualKeyCode;
			vsc = InputRecord.Event.KeyEvent.wVirtualScanCode;
			ch = InputRecord.Event.KeyEvent.uChar.AsciiChar;
			cks = InputRecord.Event.KeyEvent.dwControlKeyState;
			FlushConsoleInputBuffer(hConsoleIn);
			if ((!ch && vsc > 58))
			{
				if ((cks & 3))
				{
					return (1000 + vsc) * (-1);
				}
				if ((cks & 12))
				{
					return (2000 + vsc) * (-1);
				}
				return vsc * (-1);
			}
			if (ch && (cks & 3))
				return vkc + 1000;
			if ((vsc == 15) && (cks & 16))
				return 15;
			if (vkc == 27)
				return 27;
			if (ch && (cks & 128))
			{
				for (i = 0; i <= strlen(lchr); i++)
				{
					if (uchr[i] == ch)
					{
						ch = lchr[i];
						break;
					}
				}
			}
			if (ch)
				return ch;
		}
	} while (TRUE);
	return 0;
}

//****************************************************************

char* Escape_Message(char* buf, int code, int ior, char* prefix)
{
	char* bufp;

	if (prefix && *prefix)
	{
		strcpy(buf, prefix);
		strcat(buf, ": ");
		bufp = buf + strlen(buf);
	}
	else
	{
		bufp = buf;
	}
	if (code == -10)
	{
		sprintf(bufp, "System I/O error %d", ior);
		switch (ior)
		{
		case 3:
			strcat(buf, " (illegal I/O request)");
			break;
		case 7:
			strcat(buf, " (bad file name)");
			break;
		case 10:
			strcat(buf, " (file not found)");
			break;
		case 13:
			strcat(buf, " (file not open)");
			break;
		case 14:
			strcat(buf, " (bad input format)");
			break;
		case 24:
			strcat(buf, " (not open for reading)");
			break;
		case 25:
			strcat(buf, " (not open for writing)");
			break;
		case 26:
			strcat(buf, " (not open for direct access)");
			break;
		case 28:
			strcat(buf, " (string subscript out of range)");
			break;
		case 30:
			strcat(buf, " (end-of-file)");
			break;
		case 38:
			strcat(buf, " (file write error)");
			break;
		}
	}
	else
	{
		sprintf(bufp, "System error %d", code);

		switch (code)
		{
		case -2:
			strcat(buf, " (out of memory)");
			break;
		case -3:
			strcat(buf, " (reference to NULL pointer)");
			break;
		case -4:
			strcat(buf, " (integer overflow)");
			break;
		case -5:
			strcat(buf, " (divide by zero)");
			break;
		case -6:
			strcat(buf, " (Double math overflow)");
			break;
		case -8:
			strcat(buf, " (value range error)");
			break;
		case -9:
			strcat(buf, " (CASE value range error)");
			break;
		case -12:
			strcat(buf, " (bus error)");
			break;
		case -20:
			strcat(buf, " (stopped by user)");
			break;
		}
	}
	return buf;
}

//****************************************************************

void Critical_Escape(int code)
{
	char buf[100];
	Esc_Code = code;

	if (Top_of_Jump_Buffer)
	{
		BASIC_jmp_buf* jb = Top_of_Jump_Buffer;
		Top_of_Jump_Buffer = jb->next;
		longjmp(jb->jbuf, 1);
	}
	if (code == 0)
		exit(0);
	if (code == -1)
		exit(1);

	fprintf(stderr, "%s\n", Escape_Message(buf, Esc_Code, io_Result, ""));
	exit(1);
}

//****************************************************************

int IsNumber(char* a)
{
	int i = 0;
	if (!*a)
	{
		return FALSE;
	}
	while (a[i])
	{
		if (a[i] > 47 && a[i] < 58)
		{
			i++;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}
//****************************************************************

void Setup_Console(void)
{
	HWND hConWnd;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	hConWnd = GetConsoleWindow();
	ShowWindow(hConWnd, SW_HIDE);
	TextMode(3000);
	SetConsoleTitle("Gillespie BASIC for Windows by Kevin Diggins");
	Center(hConWnd);
	ShowWindow(hConWnd, SW_SHOW);
}

//****************************************************************

void TextMode(int Y)
{
	SMALL_RECT sr;
	COORD Coord;
	Coord.X = (SHORT)80; // Buffer columns
	Coord.Y = (SHORT)Y;  // Buffer rows
	sr.Top = 1;	          // screen position
	sr.Left = 1;   	      // screen position
	sr.Right = 80;	      // screen position
	sr.Bottom = 43;	      // screen position
	SetConsoleScreenBufferSize(hConsole, Coord);
	SetConsoleWindowInfo(hConsole, TRUE, &sr);
	color(FGCOLOR, BGCOLOR);
	cls();
}

//****************************************************************

void Cursorsh(int showFlag)	// This is the SetCursor command
{
	static HANDLE hout = { 0 };
	static CONSOLE_CURSOR_INFO cursorInfo = { 0 };
	cursorInfo.dwSize = 1;
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleCursorInfo(hout, &cursorInfo);
	cursorInfo.bVisible = showFlag;
	SetConsoleCursorInfo(hout, &cursorInfo);
}

//****************************************************************

char* mid(char* S, int start, int length)
{
	char* strtmp;
	int tmplen = (int)strlen(S);
	if (start > tmplen || start < 1)
		return BCX_TmpStr(1);
	if (length < 0 || length >(tmplen - start) + 1)
		length = (tmplen - start) + 1;
	strtmp = BCX_TmpStr(length);
	return (char*)memcpy(strtmp, &S[start - 1], length);
}

//****************************************************************
char* retain(char* Text, char* ValidChars)
{
	char* BCX_RetStr = BCX_TmpStr(strlen(Text));
	char* temp = BCX_RetStr;
	while (*Text)
	{
		if (strchr(ValidChars, *Text))
			*(temp++) = *Text;
		Text++;
	}
	return BCX_RetStr;
}
//****************************************************************
int Verify(char* Src, char* Allowed)
{
	int i, j;
	for (i = 1; i <= (int)strlen(Src); i++)
	{
		j = VerifyInstr(Allowed, mid(Src, i, 1));
		if (!j)
			return 0;
	}
	return TRUE;
}

int VerifyInstr(char* mane, char* match, int offset)
{
	char* s;
	if (!mane || !match || !*match || offset > (int)strlen(mane))
		return 0;
	s = strstr(offset > 0 ? mane + offset - 1 : mane, match);
	return s ? (int)(s - mane) + 1 : 0;
}

//****************************************************************

char* repeat(char* a, int count)
{
	char* strtmp = BCX_TmpStr((1 + count) * strlen(a));
	while (count-- > 0)
		strtmp = strcat(strtmp, a);
	return strtmp;
}

//****************************************************************

void swap(char* A, char* B, int length)
{
	char t;
	while (length--)
	{
		t = *A;
		*(A++) = *B;
		*(B++) = t;
	}
}

//****************************************************************

char* crlf(void)
{
	char* stmp = (char*)safe_calloc(3, 1);
	stmp[0] = 13;
	stmp[1] = 10;
	stmp[2] = 0;
	return stmp;
}

//****************************************************************
