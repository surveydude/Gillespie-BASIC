//*****************************************************************************
#define VERSION "Gillespie BASIC v1.61 for Windows by Kevin Diggins (01/24/2026)\n"
//              Based on Chipmunk Basic 1.0 by Dave Gillespie
//
//  LOCAL and STATICS working now
//*****************************************************************************
#define FGCOLOR 10  //  Window Foreground Color   (14)
#define BGCOLOR  0  //  Window Background Color   (01)
#define KWCOLOR  9  //  Keyword Highlight Color

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"comdlg32.lib")

#ifdef _MSC_VER
#pragma warning(disable: 4611)  // setjmp/longjmp interaction warning - safe in C
#pragma warning(disable: 4324)  // setjmp/longjmp interaction warning - safe in C 
#pragma warning(disable: 4996)  // deprecated GetVersionExA

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "Basic.h"

// Forward declarations for cmd functions
void cmdlet(BOOL implied, struct LOC_exec* LINK);
void cmdif(struct LOC_exec* LINK);
void cmdfor(struct LOC_exec* LINK);
void cmdnext(struct LOC_exec* LINK);
void cmdwhile(struct LOC_exec* LINK);
void cmdwend(struct LOC_exec* LINK);
void cmddo(struct LOC_exec* LINK);
void cmdloop(struct LOC_exec* LINK);
void cmdprint(struct LOC_exec* LINK);
void cmdsub(struct LOC_exec* LINK);
void cmdcall(struct LOC_exec* LINK);

//*****************************************************************************
// Command Dispatch Table - Single source of truth for statement handlers
// Used by both Execute() and cmdcall() to avoid code duplication
//*****************************************************************************

// Flags for command properties
#define CMD_ALLOW_IN_SUB    0x01  // Command can be used inside SUB/FUNCTION
#define CMD_IDE_ONLY        0x02  // Command is IDE-only (list, run, save, etc.)
#define CMD_SPECIAL         0x04  // Command needs special handling (not simple dispatch)
#define CMD_SKIP_NEXT       0x08  // Token already advanced past command keyword

// Command handler function pointer type
typedef void (*CmdHandler)(struct LOC_exec*);

// Command dispatch entry
typedef struct {
	int token; // Token kind (e.g., tokprint, tokgoto)
	CmdHandler handler; // Function to call (NULL if special handling needed)
	int flags; // CMD_* flags
} CmdDispatchEntry;

// Forward declarations for all cmd handlers (needed for dispatch table)
void cmdend(struct LOC_exec* LINK);
void cmdnew(struct LOC_exec* LINK);
void cmdokcancel(struct LOC_exec* LINK);
void cmdlist(struct LOC_exec* LINK);
void cmdrun(struct LOC_exec* LINK);
void cmdload(BOOL merging, char* name, struct LOC_exec* LINK);
void cmdsave(struct LOC_exec* LINK);
void cmdbye(void);
void cmddel(struct LOC_exec* LINK);
void cmdrenum(struct LOC_exec* LINK);
void cmdsleep(struct LOC_exec* LINK);
void cmdtextmode(struct LOC_exec* LINK);
void cmdrandomize(struct LOC_exec* LINK);
void cmdsetcursor(struct LOC_exec* LINK);
void cmdclose(struct LOC_exec* LINK);
void cmdrewind(struct LOC_exec* LINK);
void cmdfprint(struct LOC_exec* LINK);
void cmdget(struct LOC_exec* LINK);
void cmdput(struct LOC_exec* LINK);
void cmdseek(struct LOC_exec* LINK);
void cmdflineinput(struct LOC_exec* LINK);
void cmdswap(struct LOC_exec* LINK);
void cmdinput(struct LOC_exec* LINK);
void cmdlocal(struct LOC_exec* LINK);
void cmdstatic(struct LOC_exec* LINK);
void cmdgoto(struct LOC_exec* LINK);
void cmdelse(struct LOC_exec* LINK);
void cmdelseif(struct LOC_exec* LINK);
void cmdgosub(struct LOC_exec* LINK);
void cmdreturn(struct LOC_exec* LINK);
void cmdread(struct LOC_exec* LINK);
void cmddata(struct LOC_exec* LINK);
void cmdrestore(struct LOC_exec* LINK);
void cmdlocate(struct LOC_exec* LINK);
void cmdgotoxy(struct LOC_exec* LINK);
void cmdon(struct LOC_exec* LINK);
void cmddim(struct LOC_exec* LINK);
void cmdfunction(struct LOC_exec* LINK);
void cmdmsgbox(struct LOC_exec* LINK);
void cmdcolor(struct LOC_exec* LINK);
void cmdshell(struct LOC_exec* LINK);
void cmdeval(struct LOC_exec* LINK);
void cmdkill(struct LOC_exec* LINK);
void cmdopen(struct LOC_exec* LINK);
void cmdselect(struct LOC_exec* LINK);
void cmdcase(struct LOC_exec* LINK);
BOOL skiploop(long up, long dn, struct LOC_exec* LINK);

// The master command dispatch table
// When adding new commands, add them here and they'll work in both Execute() and SUBs
static CmdDispatchEntry cmd_dispatch[] = {
	// Standard commands - work everywhere including inside SUBs
	{ tokprint, cmdprint, CMD_ALLOW_IN_SUB },
	{ tokgoto, cmdgoto, CMD_ALLOW_IN_SUB },
	{ tokgosub, cmdgosub, CMD_ALLOW_IN_SUB },
	{ tokreturn, cmdreturn, CMD_ALLOW_IN_SUB },
	{ tokfor, cmdfor, CMD_ALLOW_IN_SUB },
	{ toknext, cmdnext, CMD_ALLOW_IN_SUB },
	{ tokwhile, cmdwhile, CMD_ALLOW_IN_SUB },
	{ tokwend, cmdwend, CMD_ALLOW_IN_SUB },
	{ tokdo, cmddo, CMD_ALLOW_IN_SUB },
	{ tokloop, cmdloop, CMD_ALLOW_IN_SUB },
	{ tokif, cmdif, CMD_ALLOW_IN_SUB },
	{ tokelse, cmdelse, CMD_ALLOW_IN_SUB },
	{ tokelseif, cmdelseif, CMD_ALLOW_IN_SUB },
	{ tokinput, cmdinput, CMD_ALLOW_IN_SUB },
	{ tokread, cmdread, CMD_ALLOW_IN_SUB },
	{ tokdata, cmddata, CMD_ALLOW_IN_SUB },
	{ tokrestore, cmdrestore, CMD_ALLOW_IN_SUB },
	{ tokdim, cmddim, CMD_ALLOW_IN_SUB },
	{ toklocal, cmdlocal, CMD_ALLOW_IN_SUB },
	{ tokstatic, cmdstatic, CMD_ALLOW_IN_SUB },
	{ tokgotoxy, cmdgotoxy, CMD_ALLOW_IN_SUB },
	{ toklocate, cmdlocate, CMD_ALLOW_IN_SUB },
	{ tokcolor, (CmdHandler)cmdcolor, CMD_ALLOW_IN_SUB },
	{ toksetcursor, cmdsetcursor, CMD_ALLOW_IN_SUB },
	{ tokmsgbox, (CmdHandler)cmdmsgbox, CMD_ALLOW_IN_SUB },
	{ tokshell, (CmdHandler)cmdshell, CMD_ALLOW_IN_SUB },
	{ tokeval, (CmdHandler)cmdeval, CMD_ALLOW_IN_SUB },
	{ tokkill, (CmdHandler)cmdkill, CMD_ALLOW_IN_SUB },
	{ tokopen, (CmdHandler)cmdopen, CMD_ALLOW_IN_SUB },
	{ tokclose, cmdclose, CMD_ALLOW_IN_SUB },
	{ tokfprint, cmdfprint, CMD_ALLOW_IN_SUB },
	{ tokflineinput, cmdflineinput, CMD_ALLOW_IN_SUB },
	{ tokget, cmdget, CMD_ALLOW_IN_SUB },
	{ tokput, cmdput, CMD_ALLOW_IN_SUB },
	{ tokseek, cmdseek, CMD_ALLOW_IN_SUB },
	{ tokrewind, cmdrewind, CMD_ALLOW_IN_SUB },
	{ tokswap, cmdswap, CMD_ALLOW_IN_SUB },
	{ tokon, cmdon, CMD_ALLOW_IN_SUB },
	{ tokrandomize, cmdrandomize, CMD_ALLOW_IN_SUB },
	{ toksleep, cmdsleep, CMD_ALLOW_IN_SUB },
	{ toktextmode, cmdtextmode, CMD_ALLOW_IN_SUB },
	{ tokcall, cmdcall, CMD_ALLOW_IN_SUB },
	{ tokokcancel, cmdokcancel, CMD_ALLOW_IN_SUB },
	{ tokselect, cmdselect, CMD_ALLOW_IN_SUB },
	{ tokcase, cmdcase, CMD_ALLOW_IN_SUB },

	// IDE-only commands - not allowed inside SUBs
	{ toklist, cmdlist, CMD_IDE_ONLY },
	{ tokrun, cmdrun, CMD_IDE_ONLY },
	{ toknew, cmdnew, CMD_IDE_ONLY },
	{ toksave, cmdsave, CMD_IDE_ONLY },
	{ tokdel, cmddel, CMD_IDE_ONLY },
	{ tokrenum, cmdrenum, CMD_IDE_ONLY },

	// Special handling commands - handled separately in Execute()/cmdcall()
	{ toklet, NULL, CMD_SPECIAL | CMD_ALLOW_IN_SUB }, // needs implied flag
	{ tokvar, NULL, CMD_SPECIAL | CMD_ALLOW_IN_SUB }, // implied let
	{ tokend, NULL, CMD_SPECIAL | CMD_ALLOW_IN_SUB }, // context-dependent
	{ tokexit, NULL, CMD_SPECIAL | CMD_ALLOW_IN_SUB }, // exit  sub/function/for
	{ tokrem, NULL, CMD_SPECIAL | CMD_ALLOW_IN_SUB }, // skip to EOL
	{ tokstop, NULL, CMD_SPECIAL | CMD_ALLOW_IN_SUB }, // halt execution
	{ tokcls, NULL, CMD_SPECIAL | CMD_ALLOW_IN_SUB }, // direct cls() call
	{ tokclear, NULL, CMD_SPECIAL }, // ClearAll()
	{ tokfunction, NULL, CMD_SPECIAL }, // define a function
	{ toksub, NULL, CMD_SPECIAL }, // define a sub
	{ tokload, NULL, CMD_SPECIAL | CMD_IDE_ONLY }, // needs a filename
	{ tokmerge, NULL, CMD_SPECIAL | CMD_IDE_ONLY }, // needs a filename
	{ tokbye, NULL, CMD_SPECIAL | CMD_IDE_ONLY }, // no LINK param
	{ toklabel, NULL, CMD_SPECIAL | CMD_ALLOW_IN_SUB }, // GOTO/GOSUB label -- skip over it
	{ 0, NULL, 0 } // End marker
};

// Lookup a command in the dispatch table
// Returns pointer to entry or NULL if not found
static CmdDispatchEntry* FindCmdDispatch(int token)
{
	for (int i = 0; cmd_dispatch[i].token != 0 || cmd_dispatch[i].handler != NULL; i++)
	{
		if (cmd_dispatch[i].token == token)
			return &cmd_dispatch[i];
	}
	return NULL;
}

// Execute a command from the dispatch table
// Returns TRUE if handled, FALSE if not found or special handling needed
static BOOL DispatchCommand(int token, struct LOC_exec* LINK, BOOL in_sub)
{
	CmdDispatchEntry* entry = FindCmdDispatch(token);

	if (entry == NULL)
		return FALSE; // Not in table

	if (entry->flags & CMD_SPECIAL)
		return FALSE; // Needs special handling

	if (in_sub && (entry->flags & CMD_IDE_ONLY))
	{
		Abort("IDE command not allowed inside SUB/FUNCTION");
	}

	if (entry->handler != NULL)
	{
		entry->handler(LINK);
		return TRUE;
	}

	return FALSE;
}

//*****************************************************************************


void Initialize(void)
{
	InitializeKeywordHashTable();
	G_inbuf = (char*)safe_calloc(MAXSTRINGVAR, 1);
	OurLoadedFname = (char*)calloc(1024, 1);
	linebase = NULL;
	varbase = NULL;
	loopbase = NULL;
	labelbase = NULL;
	auto_linenum = 10;
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
	//		strcpy(G_inbuf, "load"); // uncomment if you want OpenFile on Startup
		}
		else
		{
			// Show different prompt based on mode
			if (immediate_mode)
				printf("\n%s", "Ready: ");
			else
				printf("\n%s", "> "); // Simple prompt for program entry mode

			GB_gets(G_inbuf, MAXSTRINGVAR);
		}

		//********************************************************************
		//    This allows us to invoke the Windows GetFileName dialogbox
		//********************************************************************
		if (_stricmp(G_inbuf, "load") == 0)
			sprintf(G_inbuf, "%s%s", "load ", enc(""));

		//********************************************************************
		// Check for .. command BEFORE other processing (toggle immediate mode)
		//********************************************************************
		{
			// Make a copy to avoid modifying G_inbuf with trim
			char temp_check[64];
			strncpy(temp_check, G_inbuf, 63);
			temp_check[63] = '\0';
			char* trimmed_check = trim(temp_check);

			// Match ".." exactly to toggle the mode
			if (strcmp(trimmed_check, "..") == 0)
			{
				immediate_mode = !immediate_mode;

				if (!immediate_mode)
				{
					// Entering Program Entry Mode:
					// Find highest existing line number to continue from
					linerec* L = linebase;
					auto_linenum = 10;
					while (L != NULL)
					{
						if (L->num >= auto_linenum)
							auto_linenum = L->num + 10;
						L = L->next;
					}
					printf("Program entry mode enabled.\n");
				}
				else
				{
					// Entering Immediate Mode
					printf("Immediate mode enabled.\n");
				}
				continue;
			}

			// Match "//" exactly to toggle line number display
			if (strcmp(trimmed_check, "//") == 0)
			{
				show_linenums = !show_linenums;
				printf("Line numbers %s\n", show_linenums ? "ON" : "OFF");
				continue;
			}
		}

		//********************************************************************
		// Handle program entry mode (.. toggles this off)
		//********************************************************************
		if (!immediate_mode)
		{
			// Check if line is blank or whitespace-only
			char* p = G_inbuf;
			while (*p == ' ' || *p == '\t')
				p++; // Skip whitespace

			if (*p == '\0' || *p == '\n' || *p == '\r')
			{
				// Blank line - insert directly into linked list
				linerec* L1, * L2, * L3;
				long blank_linenum = auto_linenum;

				L3 = (linerec*)
					safe_calloc(1, sizeof(linerec));
				if (L3 == NULL)
				{
					printf("Out of memory\n");
					continue;
				}

				L3->num = blank_linenum;
				L3->has_linenum = FALSE;
				L3->txt = NULL;
				L3->next = NULL; // Important: initialize to NULL

				// Find insertion point
				L1 = linebase;
				L2 = NULL;
				while (L1 != NULL && L1->num < blank_linenum)
				{
					L2 = L1;
					L1 = L1->next;
				}

				// Insert the new node
				L3->next = L1;
				if (L2 == NULL)
					linebase = L3;
				else
					L2->next = L3;

				// Advance line counter for next line
				auto_linenum = blank_linenum + 10;

				continue; // Skip normal ParseInput processing
			}

			// Non-blank line - check if it already has a line number
			p = G_inbuf;
			while (*p == ' ' || *p == '\t')
				p++; // Skip leading whitespace

			if (!isdigit((unsigned char)*p))
			{
				// No line number - prepend auto-generated one
				char tempbuf[MAXSTRINGVAR - 1];
				long this_linenum = auto_linenum;
				sprintf(tempbuf, "%ld %s", this_linenum, G_inbuf);
				strncpy(G_inbuf, tempbuf, MAXSTRINGVAR - 1);
				G_inbuf[MAXSTRINGVAR - 1] = '\0';

				// Advance for next line
				auto_linenum = this_linenum + 10;
			}
		}

		//********************************************************************
		ParseInput(&tr_buf);

		if (curline == 0)
		{
			// Immediate mode command - execute it
			stmtline = NULL;
			stmttok = tr_buf;
			if (stmttok != NULL)
				Execute();
			DisposeTokens(&tr_buf);
		}
		else if (!immediate_mode)
		{
			// In program entry mode - mark line as auto-numbered
			linerec* L = linebase;
			while (L != NULL)
			{
				if (L->num == curline)
				{
					L->has_linenum = FALSE;
					break;
				}
				L = L->next;
			}
		}

	} while (!(exitflag));

	free(G_inbuf);
	return 0;
}




void ParseInput(tokenrec** buf)
{
	linerec* L1, * L2, * L3;
	BOOL user_provided_linenum = FALSE;
	curline = 0;

	// Safety check for trim output
	char* trimmed = trim(G_inbuf);
	if (trimmed != G_inbuf)
	{
		strncpy(G_inbuf, trimmed, MAXSTRINGVAR - 1);
		G_inbuf[MAXSTRINGVAR - 1] = '\0';
	}

	// Check if line starts with a number (user-provided line number)
	if (G_inbuf[0] != '\0' && isdigit((unsigned char)G_inbuf[0]))
	{
		user_provided_linenum = TRUE;
		while (G_inbuf[0] != '\0' && isdigit((unsigned char)G_inbuf[0]))
		{
			curline = curline * 10 + (G_inbuf[0] - '0');
			// FIX: Replaced strcpy with memmove for overlapping memory
			memmove(G_inbuf, G_inbuf + 1, strlen(G_inbuf));
		}
		// Update auto_linenum to be after the highest user line number
		if (curline >= auto_linenum)
			auto_linenum = curline + 10;
	}

	Parse(G_inbuf, buf);

	// If no line number provided but there's content, this is either:
	// 1. An immediate mode command (no line number, execute now)
	// 2. A program line without a number (auto-assign)
	// We detect immediate mode by checking if we're in "program entry" mode
	// For now, lines without numbers are immediate mode (curline stays 0)

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
		L3 = (linerec*)
			safe_calloc(1, sizeof(linerec));
		L3->next = L1;
		if (L2 == NULL)
			linebase = L3;
		else
			L2->next = L3;
		L3->num = curline;
		L3->has_linenum = user_provided_linenum;
		L3->txt = *buf;

		// Check if first token is a label (identifier followed by colon)
		if (L3->txt != NULL && L3->txt->kind == tokvar &&
			L3->txt->next != NULL && L3->txt->next->kind == tokcolon)
		{
			// Register this label
			RegisterLabel(L3->txt->vp->name, L3);
			// Mark the token as a label for display purposes
			L3->txt->kind = toklabel;
		}
	}
	ClearLoops();
	RestoreData();
}

//******************************************
//      Hash table for keyword lookup
//******************************************

#define HASH_TABLE_SIZE 128

typedef struct KeywordEntry {
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
	if (initialized)
		return;
	initialized = 1;

	// Define keyword-token pairs
	struct {
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
		{"enc$", tokenc_}, {"findfirst$", tokfindfirst_}, {"findnext$", tokfindnext_},
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
		{"append", tokappend}, {"binary", tokbinary}, {"merge", tokmerge}, {"help", tokhelp},
		{"save", toksave}, {"bye", tokbye}, {"quit", tokbye}, {"del", tokdel},
		{"renum", tokrenum},{"then", tokthen}, {"else", tokelse}, {"elseif", tokelseif}, {"to", tokto},
		{"step", tokstep}, {"clear", tokclear}, {"swap", tokswap}, {"rem", tokrem},
		{"function", tokfunction},{"exit", tokexit},{"sub", toksub}, {"call", tokcall},
		{"local", toklocal}, {"static", tokstatic},
		{"select", tokselect}, {"case", tokcase}, {"ubound", tokubound}, {"lex", toklex},
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
			if (str[i] == '-')
				d1 = 0.1;
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
		while (i < len && inbuf[i] == ' ')
			i++;
		if (i >= len)
			break;

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
			if (i < len)
				i++; // Skip closing quote
		}
		else if (ch == 39) // REM comment
		{
			t->kind = tokrem;
			t->sp = (char*)calloc(MAXSTRINGVAR, 1);
			strncpy(t->sp, inbuf + i, MAXSTRINGVAR - 1);
			break; // Rest of line is comment
		}
		else if (ch == '+')
		{
			if (i < len && inbuf[i] == '=')
			{
				t->kind = tokpluseq;
				i++;
			}
			else
				t->kind = tokplus;
		}
		else if (ch == '-')
		{
			if (i < len && inbuf[i] == '=')
			{
				t->kind = tokminuseq;
				i++;
			}
			else
				t->kind = tokminus;
		}
		else if (ch == '*')
		{
			if (i < len && inbuf[i] == '=')
			{
				t->kind = toktimeseq;
				i++;
			}
			else
				t->kind = toktimes;
		}
		else if (ch == '/')
		{
			if (i < len && inbuf[i] == '=')
			{
				t->kind = tokdiveq;
				i++;
			}
			else
				t->kind = tokdiv;
		}
		else if (ch == '^')
			t->kind = tokup;
		else if (ch == '(' || ch == '[')
			t->kind = toklp;
		else if (ch == ')' || ch == ']')
			t->kind = tokrp;
		else if (ch == ',')
			t->kind = tokcomma;
		else if (ch == ';')
			t->kind = toksemi;
		else if (ch == ':')
			t->kind = tokcolon;
		else if (ch == '?')
			t->kind = tokprint;
		else if (ch == '=')
			t->kind = tokeq;
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
			else
				t->kind = toklt;
		}
		else if (ch == '>')
		{
			if (i < len && inbuf[i] == '=')
			{
				t->kind = tokge;
				i++;
			}
			else
				t->kind = tokgt;
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
					v = (varrec*)
						calloc(sizeof(varrec), 1);
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
void ClearAll(void) // tied to the new BASIC command:  CLEAR
{
	ClearScopes(); // This will clear locals but NOT statics
	ClearStaticVars(); // NEW: Clear static variables
	ClearVars();
	ClearLoops();
	ClearLabels();
	RestoreData();
	auto_linenum = 10;
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

void ClearLabels(void)
{
	labelrec* lbl;
	labelrec* next;

	lbl = labelbase;
	while (lbl != NULL)
	{
		next = lbl->next;
		free(lbl);
		lbl = next;
	}
	labelbase = NULL;
}

//*****************************************************************************

void RegisterLabel(char* name, linerec* line)
{
	labelrec* lbl;

	// Check if label already exists
	lbl = labelbase;
	while (lbl != NULL)
	{
		if (_stricmp(lbl->name, name) == 0)
		{
			// Update existing label
			lbl->line = line;
			return;
		}
		lbl = lbl->next;
	}

	// Create new label
	lbl = (labelrec*)
		safe_calloc(1, sizeof(labelrec));
	strcpy(lbl->name, name);
	lbl->line = line;
	lbl->next = labelbase;
	labelbase = lbl;
}

//*****************************************************************************

linerec* FindLabel(char* name)
{
	labelrec* lbl;

	lbl = labelbase;
	while (lbl != NULL)
	{
		if (_stricmp(lbl->name, name) == 0)
			return lbl->line;
		lbl = lbl->next;
	}
	return NULL;
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
		while (p > s && *p == '0')
			*p-- = '\0';
		if (*p == '.')
			*p = '\0';
		strncpy(Result, ltrim(s), MAXSTRINGVAR - 1);
		return Result;
	}
}

//*****************************************************************************

void ListTokens(FILE* f, tokenrec* buf)
{
	char Str1[2048] = { 0 };
	tokenrec* start_buf = buf; // Remember start for detecting line-start tokens
	tokenrec* prev_tok = NULL; // Track previous token for context-sensitive spacing

	// Check first token for indent adjustments BEFORE printing the line
	switch (buf->kind)
	{
	case toknext:
	case tokloop:
	case tokwend:
		BumpDown();
		break;

	case toklocal:
	case tokstatic:
		// Handled in main switch below
		break;

	case tokelse:
		// ELSE at start of line - decrease indent (matching IF level)
		// Then increase again for ELSE body content
		BumpDown();
		break;

	case tokelseif:
		// ELSEIF at start of line - decrease indent (matching IF level)
		// Then increase again for ELSEIF body content
		BumpDown();
		break;

	case tokcase:
		// CASE at start of line - decrease indent (matching SELECT level)
		// Then increase again for CASE body content
		BumpDown();
		break;

	case tokend:
		// Check for END IF or END FUNCTION or END SUB or END SELECT
		if (buf->next != NULL &&
			(buf->next->kind == tokif || buf->next->kind == tokfunction || 
			 buf->next->kind == toksub || buf->next->kind == tokselect))
		{
			BumpDown();
		}
		break;
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
			fputs(", ", f);
			break;

		case tokfunction:
			HiLite(f, "FUNCTION ");
			// If FUNCTION is at start of line AND followed by a variable name (function definition)
			// (not EXIT FUNCTION or FUNCTION = value)
			if (start_buf->kind == tokfunction &&
				buf->next != NULL && buf->next->kind == tokvar)
			{
				BumpUp();
			}
			break;

		case toksub:
			HiLite(f, "SUB ");
			// If SUB is at start of line (not EXIT SUB), bump up for body
			if (start_buf->kind == toksub)
			{
				BumpUp();
			}
			break;

		case tokcall:
			HiLite(f, "CALL ");
			break;

		case tokexit:
			HiLite(f, "EXIT ");
			break;

		case toksemi:
			putc(';', f);
			break;


		case tokvar:
			fputs(mcase(buf->vp->name), f); //Use MCASE$ to list our variables
			break;

		case toknum: // numbers
			color(15, BGCOLOR);
			fputs(NumToStr(Str1, buf->num), f);
			color(FGCOLOR, BGCOLOR);
			break;

		case tokstr: // strings
			color(6, BGCOLOR);
			fprintf(f, "\"%s\"", buf->sp);
			color(FGCOLOR, BGCOLOR);
			break;

		case tokplus:
			fprintf(f, " + ");
			break;

		case tokminus:
			fprintf(f, " - ");
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
			fprintf(f, ": ");
			break;

		case tokeq:
			fprintf(f, " = ");
			break;

		case tokpluseq:
			fprintf(f, " += ");
			break;

		case tokminuseq:
			fprintf(f, " -= ");
			break;

		case toktimeseq:
			fprintf(f, " *= ");
			break;

		case tokdiveq:
			fprintf(f, " /= ");
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
			// Avoid double space after keywords that already have trailing space
			if (prev_tok != NULL && 
				(prev_tok->kind == tokif || prev_tok->kind == tokwhile ||
				 prev_tok->kind == tokelseif || prev_tok->kind == toklp ||
				 prev_tok->kind == tokprint || prev_tok->kind == tokfprint ||
				 prev_tok->kind == tokand || prev_tok->kind == tokor))
				HiLite(f, "NOT ");
			else
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

		case tokubound:
			HiLite(f, "UBOUND");
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
			HiLite(f, "SLEEP ");
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
			HiLite(f, "ENC$");
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
			if (buf->next != NULL && (buf->next->kind == tokfunction || buf->next->kind == tokif || 
			    buf->next->kind == toksub || buf->next->kind == tokselect))
				HiLite(f, "END ");
			else
				HiLite(f, "END");
			break;

		case tokstop:
			HiLite(f, "STOP");
			break;

		case tokfor:
			// Add leading space if previous token was a string (e.g., OPEN "file" FOR)
			if (prev_tok != NULL && prev_tok->kind == tokstr)
				HiLite(f, " FOR ");
			else
				HiLite(f, "FOR ");
			// Only indent if FOR is at start of line (actual loop, not EXIT FOR)
			if (start_buf->kind == tokfor)
			{
				BumpUp();
			}
			break;

		case toknext:
			HiLite(f, "NEXT ");
			break;

		case tokwhile:
			HiLite(f, "WHILE ");
			// Only indent if WHILE is at start of line (not EXIT WHILE)
			if (start_buf->kind == tokwhile)
			{
				BumpUp();
			}
			break;

		case tokwend:
			HiLite(f, "WEND ");
			break;

		case tokdo:
			HiLite(f, "DO ");
			// Only indent if DO is at start of line (not EXIT DO)
			if (start_buf->kind == tokdo)
			{
				BumpUp();
			}
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

		case toklocal:
			HiLite(f, "LOCAL ");
			break;

		case tokstatic:
			HiLite(f, "STATIC ");
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
			// Check if this is a multi-line IF (nothing meaningful after THEN)
			if (buf->next == NULL || buf->next->kind == tokrem)
			{
				BumpUp();
			}
			break;

		case tokelse:
			HiLite(f, " ELSE ");
			// If ELSE is at start of line (multi-line IF), bump up for ELSE body
			if (start_buf->kind == tokelse)
			{
				BumpUp();
			}
			break;

		case tokelseif:
			HiLite(f, "ELSEIF ");
			// Don't BumpUp here - the THEN that follows will handle it
			break;

		case tokselect:
			HiLite(f, "SELECT ");
			break;

		case tokcase:
			HiLite(f, "CASE ");
			// If CASE is at start of line, bump up for CASE body
			if (start_buf->kind == tokcase)
			{
				BumpUp();
			}
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
			fprintf(f, " ' %s", buf->sp); // All REM's become single quotes
			color(FGCOLOR, BGCOLOR);
			break;

		case toklabel:
			color(14, BGCOLOR); // Yellow for labels
			fputs(mcase(buf->vp->name), f);
			color(FGCOLOR, BGCOLOR);
			break;


		case tokSyntaxError:
			fprintf(f, ">>> %c <<<", buf->BadSyntaxChar);
			break;
		}
		prev_tok = buf;
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
	localvarrec* lv;
	tokenrec* tok;
	char* varname;

	if (LINK->Token->kind == tokrem)
		return 0; // allow apostrophe after NEXT

	if (LINK->Token == NULL || LINK->Token->kind != tokvar)
		SyntaxError();

	varname = LINK->Token->vp->name;

	//=======================================================================
	// Check for local variable FIRST
	//=======================================================================
	lv = FindLocalVar(varname);
	if (lv != NULL)
	{
		// Found a local variable - use it
		LINK->Token = LINK->Token->next;

		if (LINK->Token == NULL || LINK->Token->kind != toklp)
		{
			// Simple variable (not array access)
			if (lv->numdims != 0)
				BadSubScript(); // It's an array but no subscript given

			// Return the permanent proxy - this is stable for FOR loops etc.
			return lv->proxy;
		}

		// Array access
		if (lv->numdims == 0)
		{
			// Not declared as array - auto-dimension it
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
				lv->dims[i - 1] = 11;
			} while (LINK->Token->kind != tokrp);

			lv->numdims = (int)i;

			if (lv->IsStringVar)
			{
				lv->sarr = (char**)safe_calloc(j, sizeof(char*));
			}
			else
			{
				lv->arr = (double*)safe_calloc(j, sizeof(double));
			}
			LINK->Token = tok;
		}

		k = 0;
		LINK->Token = LINK->Token->next; // Skip '('

		FORLIM = lv->numdims;
		for (i = 1; i <= FORLIM; i++)
		{
			j = IntegerExpression(LINK);
			if ((unsigned long)j >= (unsigned long)lv->dims[i - 1])
				BadSubScript();
			k = k * lv->dims[i - 1] + j;
			if (i < lv->numdims)
				RequireToken(tokcomma, LINK);
		}

		RequireToken(tokrp, LINK);

		// Use rotating pool for array proxies too
#define LOCAL_ARR_PROXY_POOL_SIZE 16
		static varrec local_arr_proxy_pool[LOCAL_ARR_PROXY_POOL_SIZE];
		static int arr_proxy_index = 0;

		varrec* arr_proxy = &local_arr_proxy_pool[arr_proxy_index];
		arr_proxy_index = (arr_proxy_index + 1) % LOCAL_ARR_PROXY_POOL_SIZE;

		strcpy(arr_proxy->name, lv->name);
		arr_proxy->numdims = lv->numdims;
		arr_proxy->IsStringVar = lv->IsStringVar;
		if (lv->IsStringVar)
		{
			arr_proxy->AddressOfStringVar = &lv->sarr[k];
		}
		else
		{
			arr_proxy->AddressOfDoubleVar = &lv->arr[k];
		}
		return arr_proxy;
	}



	//=======================================================================
	// NEW: Check for static variable SECOND (after local, before global)
	//=======================================================================
	{
		staticvarrec* sv = FindStaticVar(varname);
		if (sv != NULL)
		{
			// Found a static variable - use it
			LINK->Token = LINK->Token->next;

			if (LINK->Token == NULL || LINK->Token->kind != toklp)
			{
				// Simple variable (not array access)
				if (sv->numdims != 0)
					BadSubScript(); // It's an array but no subscript given

				// Return the permanent proxy - this is stable for FOR loops etc.
				return sv->proxy;
			}

			// Array access
			if (sv->numdims == 0)
			{
				// Not declared as array - auto-dimension it
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
					sv->dims[i - 1] = 11;
				} while (LINK->Token->kind != tokrp);

				sv->numdims = (int)i;

				if (sv->IsStringVar)
				{
					sv->sarr = (char**)safe_calloc(j, sizeof(char*));
				}
				else
				{
					sv->arr = (double*)safe_calloc(j, sizeof(double));
				}
				LINK->Token = tok;
			}

			k = 0;
			LINK->Token = LINK->Token->next; // Skip '('

			FORLIM = sv->numdims;
			for (i = 1; i <= FORLIM; i++)
			{
				j = IntegerExpression(LINK);
				if ((unsigned long)j >= (unsigned long)sv->dims[i - 1])
					BadSubScript();
				k = k * sv->dims[i - 1] + j;
				if (i < sv->numdims)
					RequireToken(tokcomma, LINK);
			}

			RequireToken(tokrp, LINK);

			// Use rotating pool for array proxies (fine for array element access)
#define STATIC_ARR_PROXY_POOL_SIZE 16
			static varrec static_arr_proxy_pool[STATIC_ARR_PROXY_POOL_SIZE];
			static int static_arr_proxy_index = 0;

			varrec* arr_proxy = &static_arr_proxy_pool[static_arr_proxy_index];
			static_arr_proxy_index = (static_arr_proxy_index + 1) % STATIC_ARR_PROXY_POOL_SIZE;

			strcpy(arr_proxy->name, sv->name);
			arr_proxy->numdims = sv->numdims;
			arr_proxy->IsStringVar = sv->IsStringVar;
			if (sv->IsStringVar)
			{
				arr_proxy->AddressOfStringVar = &sv->sarr[k];
			}
			else
			{
				arr_proxy->AddressOfDoubleVar = &sv->arr[k];
			}
			return arr_proxy;
		}
	}

	//=======================================================================
	// ORIGINAL CODE: Global variable lookup
	//=======================================================================
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
			v->dims[i - 1] = 11;
		} while (LINK->Token->kind != tokrp);

		v->numdims = (int)i;

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
		v->AddressOfStringVar = &v->sarr[k];
	else
		v->AddressOfDoubleVar = &v->arr[k];
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
		n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1); // changed from calloc(255,1) by Kevin Diggins
		strcpy(n.sval, facttok->sp);
		break;


	case tokvar:
		LINK->Token = facttok;

		//=======================================================================
		// NEW CODE STARTS HERE - Check for local variable first
		//=======================================================================
		{
			localvarrec* lv = FindLocalVar(LINK->Token->vp->name);
			if (lv != NULL)
			{
				LINK->Token = LINK->Token->next;

				if (LINK->Token != NULL && LINK->Token->kind == toklp && lv->numdims > 0)
				{
					// Array access
					long idx = 0;
					LINK->Token = LINK->Token->next; // Skip '('

					for (int d = 0; d < lv->numdims; d++)
					{
						long subscript = IntegerExpression(LINK);
						if ((unsigned long)subscript >= (unsigned long)lv->dims[d])
							BadSubScript();
						idx = idx * lv->dims[d] + subscript;
						if (d < lv->numdims - 1)
							RequireToken(tokcomma, LINK);
					}
					RequireToken(tokrp, LINK);

					if (lv->IsStringVar)
					{
						n.IsStringVal = TRUE;
						n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
						if (lv->sarr[idx] != NULL)
							strcpy(n.sval, lv->sarr[idx]);
					}
					else
					{
						n.val = lv->arr[idx];
					}
				}
				else
				{
					// Simple variable
					if (lv->IsStringVar)
					{
						n.IsStringVal = TRUE;
						n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
						if (lv->StringVar != NULL)
							strcpy(n.sval, lv->StringVar);
					}
					else
					{
						n.val = lv->DoubleVar;
					}
				}
				break; // Exit the switch - we're done
			}
		}



		//=======================================================================
		// NEW CODE: Check for static variable 
		//=======================================================================
		{
			staticvarrec* sv = FindStaticVar(LINK->Token->vp->name);
			if (sv != NULL)
			{
				LINK->Token = LINK->Token->next;

				if (LINK->Token != NULL && LINK->Token->kind == toklp && sv->numdims > 0)
				{
					// Array access
					long idx = 0;
					LINK->Token = LINK->Token->next; // Skip '('

					for (int d = 0; d < sv->numdims; d++)
					{
						long subscript = IntegerExpression(LINK);
						if ((unsigned long)subscript >= (unsigned long)sv->dims[d])
							BadSubScript();
						idx = idx * sv->dims[d] + subscript;
						if (d < sv->numdims - 1)
							RequireToken(tokcomma, LINK);
					}
					RequireToken(tokrp, LINK);

					if (sv->IsStringVar)
					{
						n.IsStringVal = TRUE;
						n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
						if (sv->sarr[idx] != NULL)
							strcpy(n.sval, sv->sarr[idx]);
					}
					else
					{
						n.val = sv->arr[idx];
					}
				}
				else
				{
					// Simple variable
					if (sv->IsStringVar)
					{
						n.IsStringVal = TRUE;
						n.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
						if (sv->StringVar != NULL)
							strcpy(n.sval, sv->StringVar);
					}
					else
					{
						n.val = sv->DoubleVar;
					}
				}
				break; // Exit the switch - we're done
			}
		}


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

				// Check recursion depth
				if (function_call_depth >= MAX_FUNCTION_DEPTH)
					Abort("Function call depth exceeded");

				function_call_depth++;

				LINK->Token = LINK->Token->next; // Skip function name
				RequireToken(toklp, LINK);

				// Parse parameters
				int has_params = (LINK->Token != NULL && LINK->Token->kind != tokrp);
				varrec* caller_vars[MAXDIMS];
				double arg_values[MAXDIMS];
				char* arg_strings[MAXDIMS];

				if (has_params)
				{
					// FIRST PASS: Evaluate all arguments and store their values
					for (k = 0; k < f->numparams; k++)
					{
						BOOL is_string_param = (f->paramnames[k][strlen(f->paramnames[k]) - 1] == '$');
						
						// Check if argument is a simple variable
						BOOL is_simple_var = FALSE;
						if (LINK->Token != NULL && LINK->Token->kind == tokvar)
						{
							tokenrec* next_tok = LINK->Token->next;
							if (next_tok == NULL ||
								next_tok->kind == tokcomma ||
								next_tok->kind == tokrp ||
								next_tok->kind == tokcolon)
							{
								is_simple_var = TRUE;
							}
						}

						if (is_simple_var)
						{
							// Remember caller's variable for writeback
							char* arg_varname = LINK->Token->vp->name;
							localvarrec* lv = FindLocalVar(arg_varname);
							
							if (lv != NULL)
							{
								caller_vars[k] = lv->proxy;
							}
							else
							{
								staticvarrec* sv = FindStaticVar(arg_varname);
								if (sv != NULL)
								{
									caller_vars[k] = sv->proxy;
								}
								else
								{
									caller_vars[k] = LINK->Token->vp;
								}
							}

							// Read value from caller's variable
							if (is_string_param)
							{
								if (*(caller_vars[k]->AddressOfStringVar) != NULL)
									arg_strings[k] = _strdup(*(caller_vars[k]->AddressOfStringVar));
								else
									arg_strings[k] = NULL;
							}
							else
							{
								arg_values[k] = *(caller_vars[k]->AddressOfDoubleVar);
							}
							LINK->Token = LINK->Token->next;
						}
						else
						{
							// It's an expression - evaluate it
							caller_vars[k] = NULL;
							if (is_string_param)
							{
								arg_strings[k] = StringExpression(LINK);
							}
							else
							{
								arg_values[k] = DoubleExpression(LINK);
							}
						}

						if (k < f->numparams - 1)
							RequireToken(tokcomma, LINK);
					}
				}

				RequireToken(tokrp, LINK);

				// SECOND PASS: Set up parameter variables with the evaluated values
				if (has_params)
				{
					for (k = 0; k < f->numparams; k++)
					{
						varrec* param_var = varbase;

						while (param_var != NULL && _stricmp(param_var->name, f->paramnames[k]))
							param_var = param_var->next;

						if (param_var == NULL)
						{
							param_var = (varrec*)safe_calloc(1, sizeof(varrec));
							param_var->next = varbase;
							varbase = param_var;
							strcpy(param_var->name, f->paramnames[k]);
							param_var->numdims = 0;
							param_var->IsStringVar = (f->paramnames[k][strlen(f->paramnames[k]) - 1] == '$');
							if (param_var->IsStringVar)
							{
								param_var->StringVar = NULL;
								param_var->AddressOfStringVar = &param_var->StringVar;
							}
							else
							{
								param_var->DoubleVar = 0.0;
								param_var->AddressOfDoubleVar = &param_var->DoubleVar;
							}
						}

						saved_vars[k] = param_var;

						// Save the parameter's current VALUE
						if (param_var->IsStringVar)
						{
							saved_strs[k] = param_var->StringVar ? _strdup(param_var->StringVar) : NULL;
							// Now set the new value
							if (param_var->StringVar != NULL)
								free(param_var->StringVar);
							param_var->StringVar = arg_strings[k];
						}
						else
						{
							saved_vals[k] = *(param_var->AddressOfDoubleVar);
							// Now set the new value
							*(param_var->AddressOfDoubleVar) = arg_values[k];
						}
					}
				}

				// Execute function
				if (f->is_multiline)
				{
					// Multi-statement function
					linerec* saved_stmtline = stmtline;
					tokenrec* saved_stmttok = stmttok;
					looprec* saved_loopbase = loopbase;
					funcrec* saved_current_function = current_function;
					BOOL saved_function_executing = function_executing;

					// Initialize function execution context
					loopbase = NULL;
					current_function = f;
					function_executing = TRUE;
					function_exit_flag = FALSE;
					PushScope(f);


					// Initialize return value
					result.IsStringVal = f->IsStringFunc;
					if (f->IsStringFunc)
					{
						result.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
						strcpy(result.sval, "");
					}
					else
					{
						result.val = 0.0;
					}


					// Execute function body
					stmtline = f->start_line->next;
					struct LOC_exec func_exec;
					memset(&func_exec, 0, sizeof(func_exec));

					while (stmtline != NULL && stmtline != f->end_line && !function_exit_flag)
					{
						stmttok = stmtline->txt;
						if (stmttok != NULL)
						{

							func_exec.Token = stmttok;
							func_exec.gotoflag = FALSE;
							func_exec.elseflag = FALSE;

							while (func_exec.Token != NULL && !function_exit_flag)
							{

								// Skip colons
								while (func_exec.Token != NULL && func_exec.Token->kind == tokcolon)
									func_exec.Token = func_exec.Token->next;

								// If elseflag is set, skip ELSE token (single-line IF already handled it)
								if (func_exec.elseflag && func_exec.Token != NULL && func_exec.Token->kind == tokelse)
								{
									func_exec.Token = func_exec.Token->next;
									func_exec.elseflag = FALSE;
									continue;
								}

								if (func_exec.Token != NULL)
								{
									tokenrec* stmt = func_exec.Token;

									// Handle function-specific statements
									switch (stmt->kind)
									{

									case tokexit:
										// EXIT FUNCTION, EXIT FOR, EXIT WHILE, EXIT DO
										if (stmt->next != NULL && stmt->next->kind == tokfunction)
										{
											function_exit_flag = TRUE;
											func_exec.Token = stmt->next->next; // Skip both tokens
											// Clear any active loops within this function
											while (loopbase != NULL)
											{
												looprec* temp = loopbase;
												loopbase = loopbase->next;
												free(temp);
											}
										}
										else if (stmt->next != NULL && stmt->next->kind == tokfor)
										{
											// EXIT FOR
											looprec* l;
											BOOL found = FALSE;
											func_exec.Token = stmt->next->next;

											while (loopbase != NULL && !found)
											{
												if (loopbase->kind == FORLOOP)
													found = TRUE;
												else
												{
													l = loopbase->next;
													free(loopbase);
													loopbase = l;
												}
											}

											if (!found)
												Abort("EXIT FOR without FOR");
											else
											{
												if (!skiploop(tokfor, toknext, &func_exec))
													Abort("FOR without NEXT");
												l = loopbase->next;
												free(loopbase);
												loopbase = l;
											}
										}
										else if (stmt->next != NULL && stmt->next->kind == tokwhile)
										{
											// EXIT WHILE
											looprec* l;
											BOOL found = FALSE;
											func_exec.Token = stmt->next->next;

											while (loopbase != NULL && !found)
											{
												if (loopbase->kind == WHILELOOP)
													found = TRUE;
												else
												{
													l = loopbase->next;
													free(loopbase);
													loopbase = l;
												}
											}

											if (!found)
												Abort("EXIT WHILE without WHILE");
											else
											{
												if (!skiploop(tokwhile, tokwend, &func_exec))
													Abort("WHILE without WEND");
												l = loopbase->next;
												free(loopbase);
												loopbase = l;
											}
										}
										else if (stmt->next != NULL && stmt->next->kind == tokdo)
										{
											// EXIT DO
											looprec* l;
											BOOL found = FALSE;
											func_exec.Token = stmt->next->next;

											while (loopbase != NULL && !found)
											{
												if (loopbase->kind == DOLOOP)
													found = TRUE;
												else
												{
													l = loopbase->next;
													free(loopbase);
													loopbase = l;
												}
											}

											if (!found)
												Abort("EXIT DO without DO");
											else
											{
												if (!skiploop(tokdo, tokloop, &func_exec))
													Abort("DO without LOOP");
												l = loopbase->next;
												free(loopbase);
												loopbase = l;
											}
										}
										else if (stmt->next != NULL && stmt->next->kind == tokselect)
										{
											// EXIT SELECT - skip to END SELECT
											long nest = 1;
											func_exec.Token = stmt->next->next;
											
											while (nest > 0)
											{
												while (func_exec.Token == NULL)
												{
													if (stmtline == NULL || stmtline->next == NULL)
													{
														Abort("EXIT SELECT without END SELECT");
														break;
													}
													stmtline = stmtline->next;
													func_exec.Token = stmtline->txt;
												}
												if (func_exec.Token == NULL) break;

												if (func_exec.Token->kind == tokselect &&
													func_exec.Token->next != NULL &&
													func_exec.Token->next->kind == tokcase)
												{
													nest++;
												}
												else if (func_exec.Token->kind == tokend &&
													func_exec.Token->next != NULL &&
													func_exec.Token->next->kind == tokselect)
												{
													nest--;
													if (nest == 0)
													{
														func_exec.Token = func_exec.Token->next->next;
														break;
													}
													func_exec.Token = func_exec.Token->next;
												}
												func_exec.Token = func_exec.Token->next;
											}
										}
										break;

									case tokend:
										// Check what follows END
										if (stmt->next != NULL)
										{
											if (stmt->next->kind == tokif)
											{
												// END IF - just skip both tokens
												func_exec.Token = stmt->next->next;
											}
											else if (stmt->next->kind == tokselect)
											{
												// END SELECT - just skip both tokens
												func_exec.Token = stmt->next->next;
											}
											else if (stmt->next->kind == tokfunction)
											{
												// END FUNCTION - exit the function
												function_exit_flag = TRUE;
												func_exec.Token = NULL;
											}
											else
											{
												// Plain END - exit function
												function_exit_flag = TRUE;
												func_exec.Token = NULL;
											}
										}
										else
										{
											// Plain END - exit function  
											function_exit_flag = TRUE;
											func_exec.Token = NULL;
										}
										break;

									case tokvar:
										func_exec.Token = stmt;
										cmdlet(FALSE, &func_exec);
										if (_stricmp(stmt->vp->name, f->name) == 0)
										{
										}
										break;

									case toklet:
										func_exec.Token = stmt->next;
										cmdlet(FALSE, &func_exec);
										break;

									case tokif:
										func_exec.Token = stmt->next;
										cmdif(&func_exec);
										break;

									case tokelse:
										func_exec.Token = stmt->next;
										cmdelse(&func_exec);
										break;

									case tokelseif:
										func_exec.Token = stmt->next;
										cmdelseif(&func_exec);
										break;

									case tokselect:
										func_exec.Token = stmt->next;
										cmdselect(&func_exec);
										break;

									case tokcase:
										func_exec.Token = stmt->next;
										cmdcase(&func_exec);
										break;

									case tokfor:
										func_exec.Token = stmt->next;
										cmdfor(&func_exec);
										break;

									case toknext:
										func_exec.Token = stmt->next;
										cmdnext(&func_exec);
										break;

									case tokwhile:
										func_exec.Token = stmt->next;
										cmdwhile(&func_exec);
										break;

									case tokwend:
										func_exec.Token = stmt->next;
										cmdwend(&func_exec);
										break;

									case tokdo:
										func_exec.Token = stmt->next;
										cmddo(&func_exec);
										break;

									case tokloop:
										func_exec.Token = stmt->next;
										cmdloop(&func_exec);
										break;

									case tokprint:
										func_exec.Token = stmt->next;
										cmdprint(&func_exec);
										break;


									case toklocal:
										func_exec.Token = stmt->next;
										cmdlocal(&func_exec);
										break;


									case tokstatic:
										func_exec.Token = stmt->next;
										cmdstatic(&func_exec);
										break;



									case tokfunction:
										// PowerBASIC style: FUNCTION = value
										if (stmt->next != NULL && stmt->next->kind == tokeq)
										{
											// Skip past "FUNCTION ="
											func_exec.Token = stmt->next->next;

											// Find the function's return variable
											varrec* rv = varbase;
											while (rv != NULL && _stricmp(rv->name, f->name))
												rv = rv->next;

											// Create return variable if it doesn't exist
											if (rv == NULL)
											{
												rv = (varrec*)safe_calloc(1, sizeof(varrec));
												rv->next = varbase;
												varbase = rv;
												strcpy(rv->name, f->name);
												rv->numdims = 0;
												rv->IsStringVar = f->IsStringFunc;
												if (f->IsStringFunc)
												{
													rv->StringVar = NULL;
													rv->AddressOfStringVar = &rv->StringVar;
												}
												else
												{
													rv->DoubleVar = 0.0;
													rv->AddressOfDoubleVar = &rv->DoubleVar;
												}
											}

											// Assign the return value
											if (f->IsStringFunc)
											{
												char* old = rv->StringVar;
												rv->StringVar = StringExpression(&func_exec);
												if (old != NULL)
													free(old);
											}
											else
											{
												rv->DoubleVar = DoubleExpression(&func_exec);
											}
										}
										break;



									default:
										// Unknown statement - skip it
										func_exec.Token = stmt->next;
										break;

									}
								}
							}
						}

						if (!function_exit_flag && !func_exec.gotoflag)
							stmtline = stmtline->next;
					}

					// Restore execution context

					PopScope();

					stmtline = saved_stmtline;
					stmttok = saved_stmttok;
					loopbase = saved_loopbase;
					current_function = saved_current_function;
					function_executing = saved_function_executing;


					// Get the return value from the function variable
					varrec* return_var = varbase;
					while (return_var != NULL && _stricmp(return_var->name, f->name))
						return_var = return_var->next;


					if (return_var != NULL)
					{
						if (f->IsStringFunc)
						{
							if (return_var->StringVar != NULL)
							{
								result.sval = (char*)safe_calloc(MAXSTRINGVAR, 1);
								strcpy(result.sval, return_var->StringVar);
							}
						}
						else
						{
							result.val = return_var->DoubleVar;
						}
					}
				}
				else
				{
					// Single-expression function (original code)
					saved_tok = LINK->Token;
					LINK->Token = f->body;
					result = Expression(LINK);
					LINK->Token = saved_tok;
				}

				// Handle parameter cleanup
				if (has_params)
				{
					// First: capture final parameter values for writeback BEFORE restoring
					double final_vals[MAXDIMS];
					char* final_strs[MAXDIMS];
					for (k = 0; k < f->numparams; k++)
					{
						if (saved_vars[k]->IsStringVar)
						{
							final_strs[k] = saved_vars[k]->StringVar ? _strdup(saved_vars[k]->StringVar) : NULL;
						}
						else
						{
							final_vals[k] = *(saved_vars[k]->AddressOfDoubleVar);
						}
					}
					
					// Second: restore all parameters to their saved values
					for (k = 0; k < f->numparams; k++)
					{
						// Don't restore if this is the function's return variable
						if (_stricmp(saved_vars[k]->name, f->name) != 0)
						{
							if (saved_vars[k]->IsStringVar)
							{
								if (saved_vars[k]->StringVar != NULL)
									free(saved_vars[k]->StringVar);
								saved_vars[k]->StringVar = saved_strs[k];
							}
							else
							{
								*(saved_vars[k]->AddressOfDoubleVar) = saved_vals[k];
							}
						}
					}
					
					// Third: write back pass-by-reference values to caller using captured final values
					for (k = 0; k < f->numparams; k++)
					{
						if (_stricmp(saved_vars[k]->name, f->name) != 0 && caller_vars[k] != NULL)
						{
							if (saved_vars[k]->IsStringVar)
							{
								if (*(caller_vars[k]->AddressOfStringVar) != NULL)
									free(*(caller_vars[k]->AddressOfStringVar));
								*(caller_vars[k]->AddressOfStringVar) = final_strs[k]; // Transfer ownership
							}
							else
							{
								*(caller_vars[k]->AddressOfDoubleVar) = final_vals[k];
							}
						}
						else
						{
							// Not pass-by-reference or is return var, free the captured string if any
							if (saved_vars[k]->IsStringVar && final_strs[k] != NULL)
								free(final_strs[k]);
						}
					}
				}

				// Clear function_exit_flag so caller can continue
				function_exit_flag = FALSE;
				
				function_call_depth--;
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

	case tokubound:
		{
			varrec* vv;
			localvarrec* lv;
			staticvarrec* sv;
			char* varname;
			long dim_index = 1;  // Default to first dimension

			RequireToken(toklp, LINK);
			
			// Get the variable name
			if (LINK->Token == NULL || LINK->Token->kind != tokvar)
				SyntaxError();
			
			varname = LINK->Token->vp->name;
			vv = LINK->Token->vp;  // Default to global
			LINK->Token = LINK->Token->next;

			// Check for optional dimension parameter
			if (LINK->Token != NULL && LINK->Token->kind == tokcomma)
			{
				LINK->Token = LINK->Token->next;
				dim_index = intfactor(LINK);
			}

			RequireToken(tokrp, LINK);

			// Look up the variable - check locals first, then statics, then globals
			lv = FindLocalVar(varname);
			if (lv != NULL)
			{
				// Local variable
				if (lv->numdims == 0)
				{
					n.val = -1;  // Not an array
				}
				else if (dim_index < 1 || dim_index > lv->numdims)
				{
					n.val = -1;  // Invalid dimension
				}
				else
				{
					n.val = lv->dims[dim_index - 1] - 1;  // Zero-based, so max index is size-1
				}
			}
			else
			{
				sv = FindStaticVar(varname);
				if (sv != NULL)
				{
					// Static variable
					if (sv->numdims == 0)
					{
						n.val = -1;
					}
					else if (dim_index < 1 || dim_index > sv->numdims)
					{
						n.val = -1;
					}
					else
					{
						n.val = sv->dims[dim_index - 1] - 1;
					}
				}
				else
				{
					// Global variable
					if (vv->numdims == 0)
					{
						n.val = -1;  // Not an array
					}
					else if (dim_index < 1 || dim_index > vv->numdims)
					{
						n.val = -1;  // Invalid dimension
					}
					else
					{
						n.val = vv->dims[dim_index - 1] - 1;  // Zero-based
					}
				}
			}
		}
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
    printf("%s ","Extra information on line");
	//	Abort("Extra information on line");
}

//*****************************************************************************

BOOL IsEndOfStatement(struct LOC_exec* LINK)
{
    return (LINK->Token == NULL || LINK->Token->kind == tokelse || LINK->Token->kind == tokelseif || LINK->Token->kind == tokcolon || LINK->Token->kind == tokrem);
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
	// Check if this is END IF
	if (LINK->Token != NULL && LINK->Token->kind == tokif)
	{
		// This is END IF - just skip past it and continue execution
		LINK->Token = LINK->Token->next;
		return;
	}

	// Check if this is END SELECT
	if (LINK->Token != NULL && LINK->Token->kind == tokselect)
	{
		// This is END SELECT - just skip past it and continue execution
		LINK->Token = LINK->Token->next;
		return;
	}

	// Check if this is END SUB (handled by cmdcall, but just in case)
	if (LINK->Token != NULL && LINK->Token->kind == toksub)
	{
		// This is END SUB - skip past it
		LINK->Token = LINK->Token->next;
		return;
	}

	// Regular END statement - terminate program
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
	ClearLabels();
	RestoreData();
	auto_linenum = 10;
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
		funcbase = (funcrec*)
			p;
	}

	//**************
	SetConsoleTitle(VERSION);
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

		while (l != NULL && l->num <= n2)
		{
			if (l->num >= n1)
			{
				// Only show line numbers if show_linenums is TRUE or user provided them
				if (show_linenums || l->has_linenum)
				{
					printf("%ld ", l->num);
				}

				// Only call ListTokens if there are tokens (not a blank line)
				if (l->txt != NULL)
				{
					ListTokens(stdout, l->txt);
				}

				putchar('\n');
			}
			l = l->next;
		}

		if (!IsEndOfStatement(LINK))
			RequireToken(tokcomma, LINK);

	} while (!IsEndOfStatement(LINK));
}


//*****************************************************************************

void ScanForFunctions(void)
{
	linerec* L;
	tokenrec* tok;
	struct LOC_exec scan_context;

	// First, clear any existing labels and rescan
	ClearLabels();

	L = linebase;

	while (L != NULL)
	{
		tok = L->txt;

		// Look for labels (identifier followed by colon at start of line)
		if (tok != NULL && tok->kind == tokvar &&
			tok->next != NULL && tok->next->kind == tokcolon)
		{
			// Register this label and mark the token
			RegisterLabel(tok->vp->name, L);
			tok->kind = toklabel;
		}
		// Also check if already marked as label (from ParseInput)
		else if (tok != NULL && tok->kind == toklabel)
		{
			RegisterLabel(tok->vp->name, L);
		}

		// Look for FUNCTION keyword at start of line
		// Must be followed by a variable name (function definition), not "=" (return statement)
		if (tok != NULL && tok->kind == tokfunction &&
			tok->next != NULL && tok->next->kind == tokvar)

		{
			// Process this function definition
			stmtline = L;
			stmttok = tok;
			scan_context.Token = tok->next;
			scan_context.gotoflag = FALSE;
			scan_context.elseflag = FALSE;

			cmdfunction(&scan_context);
		}
		// Look for SUB keyword at start of line
		else if (tok != NULL && tok->kind == toksub)
		{
			// Process this sub definition
			stmtline = L;
			stmttok = tok;
			scan_context.Token = tok->next;
			scan_context.gotoflag = FALSE;
			scan_context.elseflag = FALSE;

			cmdsub(&scan_context);
		}

		L = L->next;
	}
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

	// Clear any stale scopes from previous runs
	ClearScopes();

	// Scan for function definitions before running
	ScanForFunctions();

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
	if (Str1[0] == 46) //LOAD "" produces Str1 = ".bas"   46 = '.'
	{
		Str1[0] = 0;
		strcpy(Str1, GetFileName("Load", "*.bas"));
		if (Str1[0] == 0)
			goto bailout; // cancelled out of GetFileName dlg
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

	// Reset auto line counter
	LineNumber = 0;

	while (fgets(G_inbuf, 2047, f) != NULL)
	{
		// Remove trailing newline
		size_t len = strlen(G_inbuf);
		if (len > 0 && G_inbuf[len - 1] == 10)
			G_inbuf[--len] = 0;
		// Also remove carriage return if present
		if (len > 0 && G_inbuf[len - 1] == 13)
			G_inbuf[--len] = 0;

		//-------------------------------------------------------
		// Check if line starts with a number (user-provided line number)
		//-------------------------------------------------------
		char* p = G_inbuf;

		// Skip leading whitespace for number check only
		while (*p == ' ' || *p == '\t')
			p++;

		if (*p != '\0' && isdigit((unsigned char)*p))
		{
			// Has a user-provided line number - use standard ParseInput
			ParseInput(&buf);
			if (curline == 0)
			{
				printf("Bad line in file\n");
				DisposeTokens(&buf);
			}
			// Update LineNumber to track highest seen
			if (curline > LineNumber)
				LineNumber = curline;
		}
		else
		{
			// No line number - auto-assign
			LineNumber += 1;

			// Check for blank/whitespace-only line
			if (*p == '\0')
			{
				// Blank line - create a linerec with no tokens directly
				linerec* L1, * L2, * L3;

				L3 = (linerec*)safe_calloc(1, sizeof(linerec));
				L3->num = LineNumber;
				L3->has_linenum = FALSE;
				L3->txt = NULL;
				L3->next = NULL;

				// Insert into list in sorted order
				L1 = linebase;
				L2 = NULL;
				while (L1 != NULL && L1->num < LineNumber)
				{
					L2 = L1;
					L1 = L1->next;
				}
				L3->next = L1;
				if (L2 == NULL)
					linebase = L3;
				else
					L2->next = L3;
			}
			else
			{
				// Non-blank line without line number
				// Prepend auto line number and parse
				char tempbuf[MAXSTRINGVAR - 1];
				sprintf(tempbuf, "%ld %s", LineNumber, G_inbuf);
				strncpy(G_inbuf, tempbuf, MAXSTRINGVAR - 1);
				G_inbuf[MAXSTRINGVAR - 1] = '\0';

				ParseInput(&buf);

				if (curline == 0)
				{
					printf("Bad line in file\n");
					DisposeTokens(&buf);
				}
				else
				{
					// Mark as auto-generated (not user-provided) line number
					linerec* L1 = linebase;
					while (L1 != NULL)
					{
						if (L1->num == LineNumber)
						{
							L1->has_linenum = FALSE;
							break;
						}
						L1 = L1->next;
					}
				}
			}
		}
	}

	// Update global auto_linenum for any future use
	auto_linenum = LineNumber + 1;

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
		// Only write line number if user originally provided one
		if (l->has_linenum)
		{
			fprintf(f, "%ld ", l->num);
		}

		// Write the tokens (blank lines have NULL txt)
		if (l->txt != NULL)
		{
			ListTokens(f, l->txt);
		}

		// Always write newline to preserve formatting
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
			if (tok->kind == tokdel || tok->kind == tokrestore || tok->kind == toklist ||
				tok->kind == tokrun || tok->kind == tokelse || tok->kind == tokthen ||
				tok->kind == tokgosub || tok->kind == tokgoto)
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
		l->has_linenum = FALSE;  // After renumbering, treat as system-assigned
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
	
	// Check if there's an argument
	if (LINK->Token == NULL || LINK->Token->kind == tokcolon || LINK->Token->kind == tokrem)
	{
		// No argument - wait for keypress
		keypress();
	}
	else
	{
		milliseconds = IntegerExpression(LINK);
		Sleep(milliseconds);
	}
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

void cmdget(struct LOC_exec* LINK) // GET fp1,A$,numofbytes
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

void cmdput(struct LOC_exec* LINK) // PUT fp1,A$,numofbytes
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

void cmdseek(struct LOC_exec* LINK) // PUT fp1,A$,numofbytes
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

void cmdflineinput(struct LOC_exec* LINK) //LINE INPUT handler
{
	FILE* FileNumber;
	varrec* v;
	char* s = (char*)safe_calloc(MAXSTRINGVAR, 1);

	RequireToken(tokinput, LINK); //swallow the INPUT keyword

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

void cmdopen(struct LOC_exec* LINK)
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
	UnSetFilePtr(FileNumber); // only one open file per FileNumber

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
	int BothAreScalar = 0; // must be 0 or 2 - otherwise an error
	int BothAreStrings = 0; // must be 0 or 2 - otherwise an error
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
		sAA = v->StringVar; //simple string
	if (v->IsStringVar && v->numdims > 0)
		sAA = (char*)v->AddressOfStringVar; //array string
	if (!v->IsStringVar && v->numdims == 0)
		AA = &v->DoubleVar; //simple number
	if (!v->IsStringVar && v->numdims > 0)
		AA = (double*)v->AddressOfDoubleVar; //array number
	//********************************************
	RequireToken(tokcomma, LINK);
	v = findvar(LINK);
	if (v->IsStringVar)
		BothAreStrings++;
	if (v->numdims == 0)
		BothAreScalar++;
	//********************************************
	if (v->IsStringVar && v->numdims == 0)
		sBB = v->StringVar; //simple string
	if (v->IsStringVar && v->numdims > 0)
		sBB = (char*)v->AddressOfStringVar; //array string
	if (!v->IsStringVar && v->numdims == 0)
		BB = &v->DoubleVar; //simple number
	if (!v->IsStringVar && v->numdims > 0)
		BB = (double*)v->AddressOfDoubleVar; //array number
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
			strcpy(*v->AddressOfStringVar, G_inbuf);
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


//*****************************************************************************
// cmdlocal - Handle LOCAL variable declarations
// Syntax: LOCAL var1, var2$, array(10), array$(5,5), ...
//*****************************************************************************
void cmdlocal(struct LOC_exec* LINK)
{
	localvarrec* lv;
	char varname[VAR_NAME_LEN + 1];
	BOOL IsStringVar;
	long i, j, k;
	BOOL done;

	// Must be inside a function or sub
	if (current_scope == NULL)
	{
		Abort("LOCAL can only be used inside FUNCTION or SUB");
		return;
	}

	do
	{
		// Expect a variable name
		if (LINK->Token == NULL || LINK->Token->kind != tokvar)
			SyntaxError();

		// Get variable name and type
		strncpy(varname, LINK->Token->vp->name, VAR_NAME_LEN);
		varname[VAR_NAME_LEN] = '\0';
		IsStringVar = (varname[strlen(varname) - 1] == '$');

		LINK->Token = LINK->Token->next;

		// Check if it's an array (has parentheses)
		if (LINK->Token != NULL && LINK->Token->kind == toklp)
		{
			// Array declaration: LOCAL arr(dim1, dim2, ...)
			lv = CreateLocalVar(varname, IsStringVar);
			if (lv == NULL)
				return;

			LINK->Token = LINK->Token->next; // Skip '('

			j = 1; // Total elements
			i = 0; // Number of dimensions

			do
			{
				k = IntegerExpression(LINK) + 1; // +1 because dim(10) means 0-10 = 11 elements

				if (k < 1)
					BadSubScript();
				if (i >= MAXDIMS)
					BadSubScript();

				lv->dims[i] = k;
				j *= k;
				i++;

				done = (LINK->Token != NULL && LINK->Token->kind == tokrp);

				if (!done)
					RequireToken(tokcomma, LINK);
			} while (!done);

 
			LINK->Token = LINK->Token->next; // Skip ')'
		 	lv->numdims = (int)i;

			// Allocate array storage (initialized to 0/NULL by safe_calloc)
			if (IsStringVar)
			{
				lv->sarr = (char**)safe_calloc(j, sizeof(char*));
			}
			else
			{
				lv->arr = (double*)safe_calloc(j, sizeof(double));
			}
		}
		else
		{
			// Simple variable: LOCAL var or LOCAL var$
			lv = CreateLocalVar(varname, IsStringVar);
			if (lv == NULL)
				return;
			// Already initialized to 0 or NULL by CreateLocalVar
		}

		// Check for more variables (comma-separated)
		if (LINK->Token != NULL && LINK->Token->kind == tokcomma)
		{
			LINK->Token = LINK->Token->next; // Skip comma
		}
		else
		{
			break; // No more variables
		}

	} while (!IsEndOfStatement(LINK));
}


//*****************************************************************************
// cmdstatic - Handle STATIC variable declarations
// Syntax: STATIC var1, var2$, array(10), array$(5,5), ...
//*****************************************************************************
void cmdstatic(struct LOC_exec* LINK)
{
    staticvarrec* sv;
    char varname[VAR_NAME_LEN + 1];
    BOOL IsStringVar;
    long i, j, k;
    BOOL done;

    // Must be inside a function or sub
    if (current_scope == NULL)
    {
        Abort("STATIC can only be used inside FUNCTION or SUB");
        return;
    }

    do
    {
        // Expect a variable name
        if (LINK->Token == NULL || LINK->Token->kind != tokvar)
            SyntaxError();

        // Get variable name and type
        strncpy(varname, LINK->Token->vp->name, VAR_NAME_LEN);
        varname[VAR_NAME_LEN] = '\0';
        IsStringVar = (varname[strlen(varname) - 1] == '$');

        LINK->Token = LINK->Token->next;

        // Check if it's an array (has parentheses)
        if (LINK->Token != NULL && LINK->Token->kind == toklp)
        {
            // Array declaration: STATIC arr(dim1, dim2, ...)

            sv = FindStaticVar(varname);

            // If the static variable already exists and we're EXECUTING the function
            if (sv != NULL && function_executing)
            {
                if (sv->numdims > 0)
                {
                    // Already dimensioned on a previous call → skip the entire dimension list
                    // This preserves the array and its values
                    LINK->Token = LINK->Token->next; // skip '('
                    int paren_count = 1;
                    while (paren_count > 0 && LINK->Token != NULL)
                    {
                        if (LINK->Token->kind == toklp) paren_count++;
                        else if (LINK->Token->kind == tokrp) paren_count--;
                        LINK->Token = LINK->Token->next;
                    }
                    // Now pointing after ')'
                    // Continue to next variable in the STATIC list
                    if (LINK->Token != NULL && LINK->Token->kind == tokcomma)
                        LINK->Token = LINK->Token->next;
                    continue;
                }
                else
                {
                    // Exists but as a scalar → type conflict
                    Abort("STATIC variable already declared as non-array");
                    return;
                }
            }

            // During definition scanning (!function_executing), skip dimensions if already exists
            if (sv != NULL && !function_executing && sv->numdims > 0)
            {
                LINK->Token = LINK->Token->next; // skip '('
                int paren_count = 1;
                while (paren_count > 0 && LINK->Token != NULL)
                {
                    if (LINK->Token->kind == toklp) paren_count++;
                    else if (LINK->Token->kind == tokrp) paren_count--;
                    LINK->Token = LINK->Token->next;
                }
                if (LINK->Token != NULL && LINK->Token->kind == tokcomma)
                    LINK->Token = LINK->Token->next;
                continue;
            }

            // If we get here: either new array or first execution → create and dimension it
            if (sv == NULL)
                sv = CreateStaticVar(varname, IsStringVar);

            LINK->Token = LINK->Token->next; // Skip '('

            // ========== Process dimensions only when EXECUTING (first call) ==========
            if (function_executing)
            {
                j = 1; // Total elements
                i = 0; // Number of dimensions
                do
                {
                    k = IntegerExpression(LINK) + 1; // +1 because dim(10) means 11 elements (0..10)
                    if (k < 1)
                        BadSubScript();
                    if (i >= MAXDIMS)
                        BadSubScript();
                    sv->dims[i] = k;
                    j *= k;
                    i++;

                    done = (LINK->Token != NULL && LINK->Token->kind == tokrp);
                    if (!done)
                        RequireToken(tokcomma, LINK);
                } while (!done);

                LINK->Token = LINK->Token->next; // Skip ')'
                sv->numdims = (int)i;

                // Allocate storage only if not already allocated
                if (IsStringVar)
                {
                    if (sv->sarr == NULL)
                    {
                        sv->sarr = (char**)safe_calloc(j, sizeof(char*));
                        for (k = 0; k < j; k++)
                            sv->sarr[k] = NULL;
                    }
                }
                else
                {
                    if (sv->arr == NULL)
                    {
                        sv->arr = (double*)safe_calloc(j, sizeof(double));
                        for (k = 0; k < j; k++)
                            sv->arr[k] = 0.0;
                    }
                }
            }
            else
            {
                // SCANNING (parsing definition) → just skip past dimensions
                int paren_count = 1;
                while (paren_count > 0 && LINK->Token != NULL)
                {
                    if (LINK->Token->kind == toklp) paren_count++;
                    else if (LINK->Token->kind == tokrp) paren_count--;
                    LINK->Token = LINK->Token->next;
                }
            }
        }
        else
        {
            // Simple scalar variable: STATIC var or STATIC var$
            sv = FindStaticVar(varname);

            if (sv != NULL && function_executing)
            {
                if (sv->IsStringVar != IsStringVar)
                {
                    Abort("STATIC variable type mismatch");
                    return;
                }
                // Already exists with correct type → do nothing (value preserved)
            }
            else if (sv == NULL)
            {
                // Create new static variable
                sv = CreateStaticVar(varname, IsStringVar);
            }
            // No initialization needed — value persists across calls
        }

        // Check for more variables (comma-separated)
        if (LINK->Token != NULL && LINK->Token->kind == tokcomma)
        {
            LINK->Token = LINK->Token->next; // Skip comma
        }
        else
        {
            break; // No more variables
        }

    } while (!IsEndOfStatement(LINK));
}

//*******************************************************************************

void cmdlet(BOOL implied, struct LOC_exec* LINK)
{
	varrec* v;
	char* old, * gnew;
	double d_value;
	double* target;
	char** starget;
	int op_kind;
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

	// Check for compound assignment operators or regular assignment
	if (LINK->Token != NULL && 
		(LINK->Token->kind == tokpluseq || LINK->Token->kind == tokminuseq ||
		 LINK->Token->kind == toktimeseq || LINK->Token->kind == tokdiveq))
	{
		op_kind = LINK->Token->kind;
		LINK->Token = LINK->Token->next;

		if (v->IsStringVar)
		{
			// String: only += is valid
			if (op_kind != tokpluseq)
			{
				Abort("Only += is valid for strings");
				return;
			}
			gnew = StringExpression(LINK);
			v->AddressOfStringVar = starget;
			old = *v->AddressOfStringVar;
			
			// Concatenate: old + new
			if (old != NULL)
			{
				char* combined = (char*)safe_calloc(strlen(old) + strlen(gnew) + 1, 1);
				strcpy(combined, old);
				strcat(combined, gnew);
				*v->AddressOfStringVar = combined;
				free(old);
				free(gnew);
			}
			else
			{
				*v->AddressOfStringVar = gnew;
			}
		}
		else
		{
			// Numeric compound assignment
			d_value = DoubleExpression(LINK);
			v->AddressOfDoubleVar = target;
			
			switch (op_kind)
			{
			case tokpluseq:
				*v->AddressOfDoubleVar += d_value;
				break;
			case tokminuseq:
				*v->AddressOfDoubleVar -= d_value;
				break;
			case toktimeseq:
				*v->AddressOfDoubleVar *= d_value;
				break;
			case tokdiveq:
				if (d_value == 0)
				{
					Abort("Division by zero");
					return;
				}
				*v->AddressOfDoubleVar /= d_value;
				break;
			}
		}
		return;
	}

	// Regular assignment
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
	linerec* target;

	// Check if target is a label (variable name) or line number
	if (LINK->Token != NULL && LINK->Token->kind == tokvar)
	{
		// It's a label
		target = FindLabel(LINK->Token->vp->name);
		if (target == NULL)
			Abort("Undefined label");
		LINK->Token = LINK->Token->next;
	}
	else
	{
		// It's a line number
		target = FindTargetLineNumber(IntegerExpression(LINK));
	}

	stmtline = target;
	LINK->Token = NULL;
	LINK->gotoflag = TRUE;
}

//*****************************************************************************

void cmdif(struct LOC_exec* LINK)
{
	double n;
	long i;
	BOOL is_multiline;

	// Reset elseflag when entering a new IF
	LINK->elseflag = FALSE;

	n = DoubleExpression(LINK);
	RequireToken(tokthen, LINK);

	// Check if this is a multi-line IF (nothing after THEN on the same line)
	is_multiline = (LINK->Token == NULL || LINK->Token->kind == tokrem);

	if (n == 0) // Condition is FALSE
	{
		if (is_multiline)
		{
			// Multi-line IF: skip lines until we find ELSEIF, ELSE, or END IF at nesting level 0
			i = 1;
			while (i > 0)
			{
				while (LINK->Token == NULL)
				{
					if (stmtline == NULL || stmtline->next == NULL)
					{
						Abort("IF without END IF");
						return;
					}
					stmtline = stmtline->next;
					LINK->Token = stmtline->txt;
				}

				if (LINK->Token->kind == tokif)
				{
					i++;
				}
				else if (LINK->Token->kind == tokend &&
					LINK->Token->next != NULL &&
					LINK->Token->next->kind == tokif)
				{
					i--;
					if (i == 0)
					{
						LINK->Token = LINK->Token->next->next;
						return;
					}
					LINK->Token = LINK->Token->next;
				}
				else if (LINK->Token->kind == tokelseif && i == 1)
				{
					// Found ELSEIF at our nesting level - evaluate its condition
					LINK->Token = LINK->Token->next; // skip past ELSEIF keyword
					n = DoubleExpression(LINK);
					RequireToken(tokthen, LINK);
					
					if (n != 0)
					{
						// ELSEIF condition is TRUE - execute this branch
						// Token is now past THEN, statements will execute
						return;
					}
					// ELSEIF condition is FALSE - continue scanning
					// Don't advance Token here; the while loop will handle it
					continue;
				}
				else if (LINK->Token->kind == tokelse && i == 1)
				{
					LINK->Token = LINK->Token->next;
					return;
				}

				LINK->Token = LINK->Token->next;
			}
		}
		else
		{
			// Single-line IF with FALSE condition: skip to after ELSE
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
			// Now positioned after ELSE - those statements will execute
			// Do NOT set elseflag - we want ELSE part to run
		}
	}
	else
	{
		// Condition is TRUE - execute THEN part
		if (!is_multiline)
		{
			if (LINK->Token != NULL && LINK->Token->kind == toknum)
			{
				cmdgoto(LINK);
			}
			else
			{
				// Set elseflag so when we hit ELSE later, we skip it
				LINK->elseflag = TRUE;
			}
		}
	}
}

//*****************************************************************************

void cmdelse(struct LOC_exec* LINK)
{
	long i;

	// Check if this ELSE is at the start of a line (multi-line IF block)
	if (stmtline != NULL && stmtline->txt != NULL &&
		stmtline->txt->kind == tokelse)
	{
		// Multi-line ELSE: we executed the THEN block, now skip to END IF
		i = 1;
		while (i > 0)
		{
			while (LINK->Token == NULL)
			{
				if (stmtline == NULL || stmtline->next == NULL)
				{
					Abort("ELSE without END IF");
					return;
				}
				stmtline = stmtline->next;
				LINK->Token = stmtline->txt;
			}

			if (LINK->Token->kind == tokend &&
				LINK->Token->next != NULL &&
				(LINK->Token->next->kind == toksub || LINK->Token->next->kind == tokfunction))
			{
				Abort("ELSE without END IF");
				return;
			}

			if (LINK->Token->kind == tokif)
			{
				i++;
			}
			else if (LINK->Token->kind == tokend &&
				LINK->Token->next != NULL &&
				LINK->Token->next->kind == tokif)
			{
				i--;
				if (i == 0)
				{
					LINK->Token = LINK->Token->next->next;
					return;
				}
				LINK->Token = LINK->Token->next;
			}

			LINK->Token = LINK->Token->next;
		}
	}
	else
	{
		// Single-line ELSE: skip to end of line
		while (LINK->Token != NULL)
		{
			LINK->Token = LINK->Token->next;
		}
	}
}

//*****************************************************************************
// ELSEIF handler - only called when we executed a previous true branch
// and need to skip to END IF
//*****************************************************************************
void cmdelseif(struct LOC_exec* LINK)
{
	long i;

	// We executed a previous IF or ELSEIF block - skip everything to END IF
	i = 1;
	while (i > 0)
	{
		while (LINK->Token == NULL)
		{
			if (stmtline == NULL || stmtline->next == NULL)
			{
				Abort("ELSEIF without END IF");
				return;
			}
			stmtline = stmtline->next;
			LINK->Token = stmtline->txt;
		}

		if (LINK->Token->kind == tokend &&
			LINK->Token->next != NULL &&
			(LINK->Token->next->kind == toksub || LINK->Token->next->kind == tokfunction))
		{
			Abort("ELSEIF without END IF");
			return;
		}

		if (LINK->Token->kind == tokif)
		{
			i++;
		}
		else if (LINK->Token->kind == tokend &&
			LINK->Token->next != NULL &&
			LINK->Token->next->kind == tokif)
		{
			i--;
			if (i == 0)
			{
				LINK->Token = LINK->Token->next->next;
				return;
			}
			LINK->Token = LINK->Token->next;
		}

		LINK->Token = LINK->Token->next;
	}
}

//*****************************************************************************
// SELECT CASE implementation
//*****************************************************************************

// Global variables to track SELECT CASE state during execution
// These are used to pass the test value between cmdselect and case matching
static double select_test_value;
static char* select_test_string = NULL;
static BOOL select_is_string;
static BOOL select_case_matched;

//*****************************************************************************
// cmdselect - Handle SELECT CASE statement
// Evaluates the test expression and finds the matching CASE
//*****************************************************************************
void cmdselect(struct LOC_exec* LINK)
{
	long i;
	double case_val;
	char* case_str;
	char test_str_buf[MAXSTRINGVAR];
	BOOL found_match;

	// Expect CASE keyword after SELECT
	if (LINK->Token == NULL || LINK->Token->kind != tokcase)
	{
		Abort("Expected CASE after SELECT");
		return;
	}
	LINK->Token = LINK->Token->next; // skip CASE

	// Determine if expression is string or numeric and evaluate it
	// Peek ahead to see if it's a string expression
	if (LINK->Token != NULL && 
		(LINK->Token->kind == tokstr || 
		 (LINK->Token->kind == tokvar && LINK->Token->vp != NULL && LINK->Token->vp->IsStringVar)))
	{
		// String expression
		select_is_string = TRUE;
		case_str = StringExpression(LINK);
		strncpy(test_str_buf, case_str, MAXSTRINGVAR - 1);
		test_str_buf[MAXSTRINGVAR - 1] = '\0';
		// Store for later use
		if (select_test_string != NULL)
			free(select_test_string);
		select_test_string = _strdup(test_str_buf);
	}
	else
	{
		// Numeric expression
		select_is_string = FALSE;
		select_test_value = DoubleExpression(LINK);
	}

	// Now scan for CASE statements and find a match
	// The structure is: SELECT CASE expr <newline> CASE val <newline> ...
	found_match = FALSE;
	i = 1; // nesting level

	while (i > 0 && !found_match)
	{
		// Advance to next line if needed
		while (LINK->Token == NULL)
		{
			if (stmtline == NULL || stmtline->next == NULL)
			{
				Abort("SELECT without END SELECT");
				return;
			}
			stmtline = stmtline->next;
			LINK->Token = stmtline->txt;
		}

		if (LINK->Token->kind == tokselect)
		{
			// Nested SELECT - skip past its CASE keyword
			if (LINK->Token->next != NULL && LINK->Token->next->kind == tokcase)
				i++;
			LINK->Token = LINK->Token->next;
		}
		else if (LINK->Token->kind == tokend &&
			LINK->Token->next != NULL &&
			LINK->Token->next->kind == tokselect)
		{
			// END SELECT
			i--;
			if (i == 0)
			{
				// No match found, exit SELECT
				LINK->Token = LINK->Token->next->next;
				return;
			}
			LINK->Token = LINK->Token->next;
		}
		else if (LINK->Token->kind == tokcase && i == 1)
		{
			LINK->Token = LINK->Token->next; // skip CASE keyword

			// Check for CASE ELSE
			if (LINK->Token != NULL && LINK->Token->kind == tokelse)
			{
				// CASE ELSE matches everything
				LINK->Token = LINK->Token->next;
				found_match = TRUE;
				select_case_matched = TRUE;
				return; // Execute CASE ELSE block
			}

			// Evaluate CASE expression(s) and compare
			// Handle multiple values: CASE 1, 2, 3
			do
			{
				if (select_is_string)
				{
					case_str = StringExpression(LINK);
					if (strcmp(select_test_string, case_str) == 0)
					{
						// Match found - skip any remaining values in this CASE
						while (LINK->Token != NULL && LINK->Token->kind == tokcomma)
						{
							LINK->Token = LINK->Token->next;
							StringExpression(LINK); // consume and discard
						}
						found_match = TRUE;
						select_case_matched = TRUE;
						return; // Execute this CASE block
					}
				}
				else
				{
					case_val = DoubleExpression(LINK);
					if (select_test_value == case_val)
					{
						// Match found - skip any remaining values in this CASE
						while (LINK->Token != NULL && LINK->Token->kind == tokcomma)
						{
							LINK->Token = LINK->Token->next;
							DoubleExpression(LINK); // consume and discard
						}
						found_match = TRUE;
						select_case_matched = TRUE;
						return; // Execute this CASE block
					}
				}

				// Check for more values (comma-separated)
				if (LINK->Token != NULL && LINK->Token->kind == tokcomma)
				{
					LINK->Token = LINK->Token->next; // skip comma
				}
				else
				{
					break; // No more values
				}
			} while (LINK->Token != NULL);

			// No match in this CASE - continue scanning
			continue;
		}

		LINK->Token = LINK->Token->next;
	}
}

//*****************************************************************************
// cmdcase - Handle CASE statement when encountered during execution
// This is called when we reach a CASE after executing a previous CASE block
// We need to skip to END SELECT
//*****************************************************************************
void cmdcase(struct LOC_exec* LINK)
{
	long i;

	// We've executed a CASE block and hit another CASE or CASE ELSE
	// Skip to END SELECT
	i = 1;
	while (i > 0)
	{
		while (LINK->Token == NULL)
		{
			if (stmtline == NULL || stmtline->next == NULL)
			{
				Abort("CASE without END SELECT");
				return;
			}
			stmtline = stmtline->next;
			LINK->Token = stmtline->txt;
		}

		if (LINK->Token->kind == tokselect &&
			LINK->Token->next != NULL &&
			LINK->Token->next->kind == tokcase)
		{
			// Nested SELECT CASE
			i++;
		}
		else if (LINK->Token->kind == tokend &&
			LINK->Token->next != NULL &&
			LINK->Token->next->kind == tokselect)
		{
			i--;
			if (i == 0)
			{
				LINK->Token = LINK->Token->next->next;
				return;
			}
			LINK->Token = LINK->Token->next;
		}

		LINK->Token = LINK->Token->next;
	}
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
		stmtline = stmtline->next; // Move past the NEXT line
		LINK->Token = NULL;
		LINK->gotoflag = TRUE;
		return;


	}
	l = (looprec*)
		safe_calloc(sizeof(looprec), 1);
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

	l = (looprec*)
		safe_calloc(sizeof(looprec), 1);
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

	l = (looprec*)
		safe_calloc(sizeof(looprec), 1);
	l->next = loopbase;
	loopbase = l;
	//*****************************************************************
	//    skip over WHILE keyword token ... added by Kevin Diggins
	//*****************************************************************

	if (!IsEndOfStatement(LINK))
		RequireToken(tokwhile, LINK);

	//*****************************************************************
	l->kind = DOLOOP;
	l->homeline = stmtline;
	l->hometok = LINK->Token;

	if (IsEndOfStatement(LINK))
		return;
	if (DoubleExpression(LINK) != 0)
		return;
	if (!skiploop(tokdo, tokloop, LINK))
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

		found = (loopbase->kind == DOLOOP);

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
	l = (looprec*)
		safe_calloc(sizeof(looprec), 1);
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

//****************************************************************

void cmdgotoxy(struct LOC_exec* LINK)
{
	int x, y;
	x = IntegerExpression(LINK);
	RequireToken(tokcomma, LINK);
	y = IntegerExpression(LINK);
	locate(y, x);
}
//****************************************************************

void cmdmsgbox(struct LOC_exec* LINK)
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

void cmdcolor(struct LOC_exec* LINK)
{
	long fg, bg;
	fg = IntegerExpression(LINK);
	RequireToken(tokcomma, LINK);
	bg = IntegerExpression(LINK);
	color(fg, bg);
}

//****************************************************************

void cmdshell(struct LOC_exec* LINK)
{
	char* Tmp1;
	Tmp1 = (char*)safe_calloc(MAXSTRINGVAR, 1);
	Tmp1 = StringExpression(LINK);
	system(Tmp1);
	free(Tmp1);
}

//****************************************************************

void cmdeval(struct LOC_exec* LINK)
{
	strcpy(G_inbuf, trim(StringExpression(LINK)));
	ParseInput(&tr_buf);
	stmttok = tr_buf; // Execute() uses a copy of buf, via stmttok
	if (stmttok != NULL)
		Execute();
	DisposeTokens(&tr_buf);
}

//****************************************************************

void cmdkill(struct LOC_exec* LINK)
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
		l = (looprec*)
			safe_calloc(sizeof(looprec), 1);
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
	if (current_scope != NULL)
	{
		cmdlocal(LINK);
		return;
	}

	long i, j, k;
	varrec* v;
	BOOL done;

	do
	{
		if (LINK->Token == NULL || LINK->Token->kind != tokvar)
			SyntaxError();

		v = LINK->Token->vp;

		LINK->Token = LINK->Token->next;

		// Check if this is a scalar declaration (no parentheses) or array
		if (LINK->Token == NULL || LINK->Token->kind != toklp)
		{
			// Scalar variable declaration - just initialize it
			v->numdims = 0;
			if (v->IsStringVar)
			{
				if (v->StringVar == NULL)
				{
					v->StringVar = NULL;
					v->AddressOfStringVar = &v->StringVar;
				}
			}
			else
			{
				v->DoubleVar = 0.0;
				v->AddressOfDoubleVar = &v->DoubleVar;
			}
		}
		else
		{
			// Array declaration with dimensions
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

			if (v->IsStringVar)
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
	linerec* L;
	tokenrec* tok;
	BOOL found_end = FALSE;

	// Allocate new function record
	f = (funcrec*)
		safe_calloc(1, sizeof(funcrec));
	f->next = funcbase;
	funcbase = f;
	f->IsSub = FALSE; // This is a FUNCTION, not a SUB
	f->statics = NULL; // Initialize static variable list

	// Get function name
	if (LINK->Token == NULL || LINK->Token->kind != tokvar)
		SyntaxError();

	v = LINK->Token->vp;
	strcpy(f->name, v->name);
	f->IsStringFunc = v->IsStringVar;

	LINK->Token = LINK->Token->next;

	// Parse parameters
	f->numparams = 0;
	if (LINK->Token != NULL && LINK->Token->kind == toklp)
	{
		LINK->Token = LINK->Token->next;

		if (LINK->Token != NULL && LINK->Token->kind != tokrp)
		{
			while (LINK->Token != NULL && LINK->Token->kind != tokrp)
			{
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

		RequireToken(tokrp, LINK);
	}

	// Check if this is a single-expression function (has = sign)
	if (LINK->Token != NULL && LINK->Token->kind == tokeq)
	{
		// Single-expression function
		f->is_multiline = FALSE;
		LINK->Token = LINK->Token->next;

		tokenrec* body_start = NULL;
		tokenrec* prev = NULL;

		while (LINK->Token != NULL && LINK->Token->kind != tokcolon)
		{
			tokenrec* t = (tokenrec*)safe_calloc(1, sizeof(tokenrec));

			t->kind = LINK->Token->kind;
			t->vp = LINK->Token->vp;
			t->num = LINK->Token->num;
			t->BadSyntaxChar = LINK->Token->BadSyntaxChar;
			t->next = NULL;

			if (t->kind == tokstr || t->kind == tokrem)
			{
				t->sp = (char*)safe_calloc(MAXSTRINGVAR, 1);
				strcpy(t->sp, LINK->Token->sp);
			}
			else
			{
				t->sp = NULL;
			}

			if (body_start == NULL)
			{
				body_start = t;
			}
			else
			{
				prev->next = t;
			}
			prev = t;

			LINK->Token = LINK->Token->next;
		}

		f->body = body_start;
		f->start_line = NULL;
		f->end_line = NULL;
	}
	else
	{
		// Multi-statement function - find END FUNCTION
		f->is_multiline = TRUE;
		f->body = NULL;
		f->start_line = stmtline;

		// Search for matching END FUNCTION (starting from next line)
		L = stmtline->next;

		while (L != NULL)
		{
			tok = L->txt;
			// Look for END FUNCTION sequence
			while (tok != NULL)
			{
				if (tok->kind == tokend && tok->next != NULL && tok->next->kind == tokfunction)
				{
					f->end_line = L;
					found_end = TRUE;
					break;
				}
				tok = tok->next;
			}
			if (found_end)
				break;
			L = L->next;
		}

		if (!found_end)
		{
			Abort("FUNCTION without END FUNCTION");
		}
	}
}

//*****************************************************************************

void cmdsub(struct LOC_exec* LINK)
{
	funcrec* f;
	varrec* v;
	linerec* L;
	tokenrec* tok;
	BOOL found_end = FALSE;

	// Allocate new sub record (using funcrec structure)
	f = (funcrec*)
		safe_calloc(1, sizeof(funcrec));
	f->next = funcbase;
	funcbase = f;
	f->IsSub = TRUE; // This is a SUB, not a FUNCTION
	f->IsStringFunc = FALSE; // SUBs don't return values
	f->statics = NULL; // Initialize static variable list

	// Get sub name
	if (LINK->Token == NULL || LINK->Token->kind != tokvar)
		SyntaxError();

	v = LINK->Token->vp;
	strcpy(f->name, v->name);

	LINK->Token = LINK->Token->next;

	// Parse parameters
	f->numparams = 0;
	if (LINK->Token != NULL && LINK->Token->kind == toklp)
	{
		LINK->Token = LINK->Token->next;

		if (LINK->Token != NULL && LINK->Token->kind != tokrp)
		{
			while (LINK->Token != NULL && LINK->Token->kind != tokrp)
			{
				if (LINK->Token->kind != tokvar)
					SyntaxError();

				if (f->numparams >= MAXDIMS)
					Abort("Too many SUB parameters");

				strcpy(f->paramnames[f->numparams], LINK->Token->vp->name);
				f->numparams++;

				LINK->Token = LINK->Token->next;

				if (LINK->Token != NULL && LINK->Token->kind == tokcomma)
					LINK->Token = LINK->Token->next;
			}
		}

		RequireToken(tokrp, LINK);
	}

	// SUBs are always multi-line - find END SUB
	f->is_multiline = TRUE;
	f->body = NULL;
	f->start_line = stmtline;

	// Search for matching END SUB (starting from next line)
	L = stmtline->next;

	while (L != NULL)
	{
		tok = L->txt;
		// Look for END SUB sequence
		while (tok != NULL)
		{
			if (tok->kind == tokend && tok->next != NULL && tok->next->kind == toksub)
			{
				f->end_line = L;
				found_end = TRUE;
				break;
			}
			tok = tok->next;
		}
		if (found_end)
			break;
		L = L->next;
	}

	if (!found_end)
	{
		Abort("SUB without END SUB");
	}
}

//*****************************************************************************

void cmdcall(struct LOC_exec* LINK)
{
	funcrec* f;
	varrec* saved_vars[MAXDIMS];
	double saved_vals[MAXDIMS];
	char* saved_strs[MAXDIMS];
	int k;

	// Get SUB name
	if (LINK->Token == NULL || LINK->Token->kind != tokvar)
		SyntaxError();

	f = FindFunction(LINK->Token->vp->name);
	if (f == NULL)
		Abort("Undefined SUB");

	if (!f->IsSub)
		Abort("CALL requires a SUB, not a FUNCTION");

	// Check recursion depth
	if (function_call_depth >= MAX_FUNCTION_DEPTH)
		Abort("SUB call depth exceeded");

	function_call_depth++;

	LINK->Token = LINK->Token->next; // Skip SUB name

	// Parse arguments if present
	int has_params = (LINK->Token != NULL && LINK->Token->kind == toklp);
	varrec* caller_vars[MAXDIMS]; // The caller's actual variables (for pass-by-reference writeback)
	double arg_values[MAXDIMS];   // Evaluated argument values
	char* arg_strings[MAXDIMS];   // Evaluated string argument values

	if (has_params)
	{
		RequireToken(toklp, LINK);

		// FIRST PASS: Evaluate all arguments and store their values
		for (k = 0; k < f->numparams; k++)
		{
			BOOL is_string_param = (f->paramnames[k][strlen(f->paramnames[k]) - 1] == '$');
			
			// Check if argument is a simple variable (for pass-by-reference writeback)
			BOOL is_simple_var = FALSE;
			if (LINK->Token != NULL && LINK->Token->kind == tokvar)
			{
				tokenrec* next_tok = LINK->Token->next;
				if (next_tok == NULL ||
					next_tok->kind == tokcomma ||
					next_tok->kind == tokrp ||
					next_tok->kind == tokcolon)
				{
					is_simple_var = TRUE;
				}
			}

			if (is_simple_var)
			{
				// Remember caller's variable for writeback after call
				char* arg_varname = LINK->Token->vp->name;
				localvarrec* lv = FindLocalVar(arg_varname);
				
				if (lv != NULL)
				{
					caller_vars[k] = lv->proxy;
				}
				else
				{
					staticvarrec* sv = FindStaticVar(arg_varname);
					if (sv != NULL)
					{
						caller_vars[k] = sv->proxy;
					}
					else
					{
						caller_vars[k] = LINK->Token->vp;
					}
				}

				// Read value from caller's variable
				if (is_string_param)
				{
					if (*(caller_vars[k]->AddressOfStringVar) != NULL)
						arg_strings[k] = _strdup(*(caller_vars[k]->AddressOfStringVar));
					else
						arg_strings[k] = NULL;
				}
				else
				{
					arg_values[k] = *(caller_vars[k]->AddressOfDoubleVar);
				}
				LINK->Token = LINK->Token->next;
			}
			else
			{
				// It's an expression - evaluate it
				caller_vars[k] = NULL;
				if (is_string_param)
				{
					arg_strings[k] = StringExpression(LINK);
				}
				else
				{
					arg_values[k] = DoubleExpression(LINK);
				}
			}

			if (k < f->numparams - 1)
				RequireToken(tokcomma, LINK);
		}

		RequireToken(tokrp, LINK);

		// SECOND PASS: Set up parameter variables with the evaluated values
		for (k = 0; k < f->numparams; k++)
		{
			// Get or create the parameter variable
			varrec* param_var = varbase;
			while (param_var != NULL && _stricmp(param_var->name, f->paramnames[k]))
				param_var = param_var->next;

			if (param_var == NULL)
			{
				param_var = (varrec*)safe_calloc(1, sizeof(varrec));
				param_var->next = varbase;
				varbase = param_var;
				strcpy(param_var->name, f->paramnames[k]);
				param_var->numdims = 0;
				param_var->IsStringVar = (f->paramnames[k][strlen(f->paramnames[k]) - 1] == '$');
				if (param_var->IsStringVar)
				{
					param_var->StringVar = NULL;
					param_var->AddressOfStringVar = &param_var->StringVar;
				}
				else
				{
					param_var->DoubleVar = 0.0;
					param_var->AddressOfDoubleVar = &param_var->DoubleVar;
				}
			}

			saved_vars[k] = param_var;

			// Save the parameter's current VALUE (for restoration after call)
			if (param_var->IsStringVar)
			{
				saved_strs[k] = param_var->StringVar ? _strdup(param_var->StringVar) : NULL;
				// Now set the new value
				if (param_var->StringVar != NULL)
					free(param_var->StringVar);
				param_var->StringVar = arg_strings[k];
			}
			else
			{
				saved_vals[k] = *(param_var->AddressOfDoubleVar);
				// Now set the new value
				*(param_var->AddressOfDoubleVar) = arg_values[k];
			}
		}
	}

	// Execute SUB body
	linerec* saved_stmtline = stmtline;
	tokenrec* saved_stmttok = stmttok;
	looprec* saved_loopbase = loopbase;
	funcrec* saved_current_function = current_function;
	BOOL saved_function_executing = function_executing;

	// Initialize SUB execution context
	loopbase = NULL;
	current_function = f;
	function_executing = TRUE;
	function_exit_flag = FALSE;

	PushScope(f);

	// Execute SUB body
	stmtline = f->start_line->next;
	struct LOC_exec sub_exec;
	memset(&sub_exec, 0, sizeof(sub_exec));

	while (stmtline != NULL && stmtline != f->end_line && !function_exit_flag)
	{
		stmttok = stmtline->txt;
		if (stmttok != NULL)
		{
			sub_exec.Token = stmttok;
			sub_exec.gotoflag = FALSE;
			sub_exec.elseflag = FALSE;

			while (sub_exec.Token != NULL && !function_exit_flag)
			{
				// Skip colons
				while (sub_exec.Token != NULL && sub_exec.Token->kind == tokcolon)
					sub_exec.Token = sub_exec.Token->next;

				// If elseflag is set, skip ELSE token (single-line IF already handled it)
				if (sub_exec.elseflag && sub_exec.Token != NULL && sub_exec.Token->kind == tokelse)
				{
					sub_exec.Token = sub_exec.Token->next;
					sub_exec.elseflag = FALSE;
					continue;
				}

				if (sub_exec.Token != NULL)
				{
					tokenrec* stmt = sub_exec.Token;
					int tok_kind = stmt->kind;

					// Try the dispatch table first for standard commands
					sub_exec.Token = stmt->next; // Advance past command token
					if (DispatchCommand(tok_kind, &sub_exec, TRUE))
					{
						// Command was handled by dispatch table
					}
					// Handle special cases that need custom logic
					else
						switch (tok_kind)
						{

						case tokexit:
							// EXIT SUB, EXIT FOR, EXIT WHILE, EXIT DO
							if (stmt->next != NULL && stmt->next->kind == toksub)
							{
								function_exit_flag = TRUE;
								sub_exec.Token = stmt->next->next;
								// Clear any active loops within this SUB
								while (loopbase != NULL)
								{
									looprec* temp = loopbase;
									loopbase = loopbase->next;
									free(temp);
								}
							}
							else if (stmt->next != NULL && stmt->next->kind == tokfor)
							{
								// EXIT FOR
								looprec* l;
								BOOL found = FALSE;
								sub_exec.Token = stmt->next->next;

								while (loopbase != NULL && !found)
								{
									if (loopbase->kind == FORLOOP)
										found = TRUE;
									else
									{
										l = loopbase->next;
										free(loopbase);
										loopbase = l;
									}
								}

								if (!found)
									Abort("EXIT FOR without FOR");
								else
								{
									if (!skiploop(tokfor, toknext, &sub_exec))
										Abort("FOR without NEXT");
									l = loopbase->next;
									free(loopbase);
									loopbase = l;
								}
							}
							else if (stmt->next != NULL && stmt->next->kind == tokwhile)
							{
								// EXIT WHILE
								looprec* l;
								BOOL found = FALSE;
								sub_exec.Token = stmt->next->next;

								while (loopbase != NULL && !found)
								{
									if (loopbase->kind == WHILELOOP)
										found = TRUE;
									else
									{
										l = loopbase->next;
										free(loopbase);
										loopbase = l;
									}
								}

								if (!found)
									Abort("EXIT WHILE without WHILE");
								else
								{
									if (!skiploop(tokwhile, tokwend, &sub_exec))
										Abort("WHILE without WEND");
									l = loopbase->next;
									free(loopbase);
									loopbase = l;
								}
							}
							else if (stmt->next != NULL && stmt->next->kind == tokdo)
							{
								// EXIT DO
								looprec* l;
								BOOL found = FALSE;
								sub_exec.Token = stmt->next->next;

								while (loopbase != NULL && !found)
								{
									if (loopbase->kind == DOLOOP)
										found = TRUE;
									else
									{
										l = loopbase->next;
										free(loopbase);
										loopbase = l;
									}
								}

								if (!found)
									Abort("EXIT DO without DO");
								else
								{
									if (!skiploop(tokdo, tokloop, &sub_exec))
										Abort("DO without LOOP");
									l = loopbase->next;
									free(loopbase);
									loopbase = l;
								}
							}
							else if (stmt->next != NULL && stmt->next->kind == tokselect)
							{
								// EXIT SELECT - skip to END SELECT
								long nest = 1;
								sub_exec.Token = stmt->next->next;
								
								while (nest > 0)
								{
									while (sub_exec.Token == NULL)
									{
										if (stmtline == NULL || stmtline->next == NULL)
										{
											Abort("EXIT SELECT without END SELECT");
											break;
										}
										stmtline = stmtline->next;
										sub_exec.Token = stmtline->txt;
									}
									if (sub_exec.Token == NULL) break;

									if (sub_exec.Token->kind == tokselect &&
										sub_exec.Token->next != NULL &&
										sub_exec.Token->next->kind == tokcase)
									{
										nest++;
									}
									else if (sub_exec.Token->kind == tokend &&
										sub_exec.Token->next != NULL &&
										sub_exec.Token->next->kind == tokselect)
									{
										nest--;
										if (nest == 0)
										{
											sub_exec.Token = sub_exec.Token->next->next;
											break;
										}
										sub_exec.Token = sub_exec.Token->next;
									}
									sub_exec.Token = sub_exec.Token->next;
								}
							}
							break;

						case tokend:
							// Check what follows END
							if (stmt->next != NULL)
							{
								if (stmt->next->kind == tokif)
								{
									// END IF - just skip both tokens
									sub_exec.Token = stmt->next->next;
								}
								else if (stmt->next->kind == tokselect)
								{
									// END SELECT - just skip both tokens
									sub_exec.Token = stmt->next->next;
								}
								else if (stmt->next->kind == toksub)
								{
									// END SUB - exit the SUB
									function_exit_flag = TRUE;
									sub_exec.Token = NULL;
								}
								else
								{
									// Plain END - exit SUB
									function_exit_flag = TRUE;
									sub_exec.Token = NULL;
								}
							}
							else
							{
								// Plain END - exit SUB
								function_exit_flag = TRUE;
								sub_exec.Token = NULL;
							}
							break;

						case tokvar:
							sub_exec.Token = stmt; // Reset - cmdlet needs the var token
							cmdlet(FALSE, &sub_exec);
							break;

						case toklet:
							// Token already advanced
							cmdlet(FALSE, &sub_exec);
							break;

						case tokrem:
							// Comment - skip to end of line
							sub_exec.Token = NULL;
							break;

						case tokstop:
							// STOP - halt execution
							Esc_Code = -20;
							function_exit_flag = TRUE;
							sub_exec.Token = NULL;
							break;

						case tokcls:
							cls();
							// Token already advanced
							break;

						case toklabel:
							// Label definition - just skip it (token already advanced)
							break;

						default:
							// Unknown statement - report syntax error
							SyntaxError();
							break;
						}
				}
			}
		}

		if (!function_exit_flag && !sub_exec.gotoflag)
			stmtline = stmtline->next;
	}

	// Restore execution context

	PopScope();

	stmtline = saved_stmtline;
	stmttok = saved_stmttok;
	loopbase = saved_loopbase;
	current_function = saved_current_function;
	function_executing = saved_function_executing;

	// Handle parameter cleanup
	if (has_params)
	{
		// First: capture final parameter values for writeback BEFORE restoring
		double final_vals[MAXDIMS];
		char* final_strs[MAXDIMS];
		for (k = 0; k < f->numparams; k++)
		{
			if (saved_vars[k]->IsStringVar)
			{
				final_strs[k] = saved_vars[k]->StringVar ? _strdup(saved_vars[k]->StringVar) : NULL;
			}
			else
			{
				final_vals[k] = *(saved_vars[k]->AddressOfDoubleVar);
			}
		}
		
		// Second: restore all parameters to their saved values
		for (k = 0; k < f->numparams; k++)
		{
			if (saved_vars[k]->IsStringVar)
			{
				if (saved_vars[k]->StringVar != NULL)
					free(saved_vars[k]->StringVar);
				saved_vars[k]->StringVar = saved_strs[k];
			}
			else
			{
				*(saved_vars[k]->AddressOfDoubleVar) = saved_vals[k];
			}
		}
		
		// Third: write back pass-by-reference values to caller using captured final values
		for (k = 0; k < f->numparams; k++)
		{
			if (caller_vars[k] != NULL)
			{
				if (saved_vars[k]->IsStringVar)
				{
					if (*(caller_vars[k]->AddressOfStringVar) != NULL)
						free(*(caller_vars[k]->AddressOfStringVar));
					*(caller_vars[k]->AddressOfStringVar) = final_strs[k]; // Transfer ownership
				}
				else
				{
					*(caller_vars[k]->AddressOfDoubleVar) = final_vals[k];
				}
			}
			else
			{
				// Not pass-by-reference, free the captured string if any
				if (saved_vars[k]->IsStringVar && final_strs[k] != NULL)
					free(final_strs[k]);
			}
		}
	}

	// Clear function_exit_flag so caller can continue
	function_exit_flag = FALSE;
	
	function_call_depth--;
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

	if (f == NULL)
	{
		printf("No user-defined functions or subs.\n");
		return;
	}

	printf("\n--- User-Defined Functions and Subs ---\n");

	while (f != NULL)
	{
		color(KWCOLOR, BGCOLOR);
		if (f->IsSub)
		{
			printf("SUB ");
		}
		else
		{
			printf("FUNCTION ");
		}
		color(FGCOLOR, BGCOLOR);
		printf("%s", mcase(f->name));

		if (f->numparams > 0)
		{
			printf("(");
			for (i = 0; i < f->numparams; i++)
			{
				printf("%s", mcase(f->paramnames[i]));
				if (i < f->numparams - 1)
					printf(", ");
			}
			printf(")");
		}

		if (!f->IsSub && !f->is_multiline && f->body != NULL)
		{
			printf(" = ");
			ListTokens(stdout, f->body);
		}
		else if (f->is_multiline)
		{
			printf(" [lines %ld-%ld]", f->start_line->num, f->end_line->num);
		}
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
                    color(FGCOLOR, BGCOLOR);
                    Cursorsh(1);
					Esc_Code = -20;
					goto _Ltry1;
				}
			}
			//****************************************************************
			V.gotoflag = FALSE;

			// If elseflag is set from previous IF statement, skip entire ELSE clause
			if (V.elseflag && stmttok != NULL && stmttok->kind == tokelse)
			{
				// Skip ELSE and ALL tokens after it until end of line or colon
				while (stmttok != NULL && stmttok->kind != tokcolon)
				{
					stmttok = stmttok->next;
				}
				V.elseflag = FALSE;
			}

			while (stmttok != NULL && stmttok->kind == tokcolon)
				stmttok = stmttok->next;
			V.Token = stmttok;

			// Reset elseflag if we've reached end of line without hitting ELSE
			if (stmttok == NULL)
				V.elseflag = FALSE;

			if (V.Token != NULL)
			{
				V.Token = V.Token->next;
				switch (stmttok->kind)
				{
				case tokrem:
					/* blank case */
					break;


				case toklocal:
					cmdlocal(&V);
					break;

				case tokstatic:
					cmdstatic(&V);
					break;


				case toklabel:
					// Labels are just markers - skip past the colon and continue
					if (V.Token != NULL && V.Token->kind == tokcolon)
						V.Token = V.Token->next;
					break;

				case tokfunction:
					// If we hit a FUNCTION during normal execution, skip to END FUNCTION
				{
					linerec* func_start = stmtline;
					while (stmtline != NULL)
					{
						tokenrec* t = stmtline->txt;
						while (t != NULL)
						{
							if (t->kind == tokend && t->next != NULL && t->next->kind == tokfunction)
							{
								// Found END FUNCTION - go to next line
								stmtline = stmtline->next;
								if (stmtline != NULL)
									stmttok = stmtline->txt;
								V.Token = NULL;
								V.gotoflag = TRUE;
								goto function_skipped;
							}
							t = t->next;
						}
						stmtline = stmtline->next;
					}
					// If we get here, no END FUNCTION found
					stmtline = func_start;
					Abort("FUNCTION without END FUNCTION");
				}
			function_skipped:
				break;

				case toksub:
					// If we hit a SUB during normal execution, skip to END SUB
				{
					linerec* sub_start = stmtline;
					while (stmtline != NULL)
					{
						tokenrec* t = stmtline->txt;
						while (t != NULL)
						{
							if (t->kind == tokend && t->next != NULL && t->next->kind == toksub)
							{
								// Found END SUB - go to next line
								stmtline = stmtline->next;
								if (stmtline != NULL)
									stmttok = stmtline->txt;
								V.Token = NULL;
								V.gotoflag = TRUE;
								goto sub_skipped;
							}
							t = t->next;
						}
						stmtline = stmtline->next;
					}
					// If we get here, no END SUB found
					stmtline = sub_start;
					Abort("SUB without END SUB");
				}
			sub_skipped:
				break;

				case tokcall:
					cmdcall(&V);
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

				case tokhelp:
					printf("\n");
					printf("Immediate Mode Commands:\n");
					printf("------------------------\n");
					printf("BYE      - Exit the interpreter\n");
					printf("CLEAR    - Clear all variables\n");
					printf("DEL n-m  - Delete lines n through m\n");
					printf("HELP     - Show this help\n");
					printf("LEX      - Display the Lexicon (Commands and Functions)\n");
					printf("LIST     - List program (LIST n-m for range)\n");
					printf("LOAD     - Load a program from disk\n");
					printf("MERGE    - Merge a program from disk\n");
					printf("NEW      - Clear program and variables\n");
					printf("RENUM    - Renumber program lines\n");
					printf("RUN      - Run the program\n");
					printf("SAVE     - Save program to disk\n");
					printf("..       - Toggle immediate mode ON/OFF\n");
					printf("//       - Toggle line number display ON/OFF\n");

					printf("\n");
					break;

				case toklex:
					printf("\n");
					printf("===============================================================================\n");
				    color(11, BGCOLOR);
                	printf("              GILLESPIE BASIC INTERPRETER - KEYWORD LEXICON\n");
                    color(FGCOLOR, BGCOLOR);
					printf("===============================================================================\n");
					printf("\n");
                    color(11, BGCOLOR);
					printf("STATEMENTS & COMMANDS\n");
                    color(FGCOLOR, BGCOLOR);
					printf("---------------------------------------\n");
					printf("CALL name(args)      - Call a SUB\n");
					printf("CLEAR                - Clear all variables\n");
					printf("CLOSE #n             - Close a file\n");
					printf("CLS                  - Clear screen\n");
					printf("COLOR fg, bg         - Set text colors\n");
					printf("DATA val, val, ...   - Define data for READ\n");
					printf("DEL n-m              - Delete line numbers (IDE)\n");
					printf("DIM var(n)           - Dimension an array\n");
					printf("DO ... LOOP          - Loop structure\n");
					printf("DO ... LOOP UNTIL    - Loop with condition\n");
					printf("END                  - End program\n");
					printf("END FUNCTION         - End a function definition\n");
					printf("END IF               - End IF block\n");
					printf("END SELECT           - End SELECT block\n");
					printf("END SUB              - End a SUB definition\n");
					printf("EXIT FOR/DO/WHILE    - Exit a loop\n");
					printf("EXIT SELECT          - Exit a SELECT block\n");
					printf("EXIT FUNCTION/SUB    - Exit a FUNCTION or SUB\n");
					printf("FOR var = n TO m [STEP s] - FOR loop\n");
					printf("FPRINT #n, expr      - Print to file\n");
					printf("FUNCTION name(args)  - Define a function\n");
					printf("GET #n, rec, var     - Read binary record\n");
					printf("GOSUB label          - Call subroutine\n");
					printf("GOTO label           - Jump to label\n");
					printf("GOTOXY x, y          - Position cursor\n");
					printf("IF...THEN...ELSEIF...ELSE...END IF - Conditional\n");
					printf("INPUT [prompt,] var  - Get user input\n");
					printf("KILL filename        - Delete a file\n");
					printf("LET var = expr       - Assignment (LET optional)\n");
					printf("LINE INPUT #n, var$  - Read line from file\n");
					printf("LIST [n-m]           - List program (IDE)\n");
					printf("LOAD                 - Load program (IDE)\n");
					printf("LOCAL var            - Declare local variable\n");
					printf("LOCATE row, col      - Position cursor\n");
					printf("MERGE                - Merge program (IDE)\n");
					printf("MSGBOX \"text\"        - Display message box\n");
					printf("NEW                  - Clear program (IDE)\n");
					printf("NEXT var             - End of FOR loop\n");
					printf("ON n GOTO/GOSUB ...  - Computed branch\n");
					printf("OPEN file FOR mode AS #n - Open a file\n");
					printf("PRINT expr           - Print to screen\n");
					printf("PUT #n, rec, var     - Write binary record\n");
					printf("QUIT / BYE           - Exit interpreter (IDE)\n");
					printf("RANDOMIZE            - Seed random generator\n");
					printf("READ var             - Read from DATA\n");
					printf("REM comment (or ')   - Comment\n");
					printf("RENUM                - Renumber lines (IDE)\n");
					printf("RESTORE              - Reset DATA pointer\n");
					printf("RETURN               - Return from GOSUB\n");
					printf("REWIND #n            - Rewind file to start\n");
					printf("RUN                  - Run program (IDE)\n");
					printf("SAVE                 - Save program (IDE)\n");
					printf("SEEK #n, pos         - Set file position\n");
					printf("SELECT CASE expr     - Multi-way branch\n");
					printf("  CASE val [,val...] - Case in SELECT\n");
					printf("  CASE ELSE          - Default case\n");
					printf("SETCURSOR n          - Set cursor visibility\n");
					printf("SHELL \"command\"      - Execute system command\n");
					printf("SLEEP [milliseconds] - Pause (or wait for key)\n");
					printf("STATIC var           - Declare static variable\n");
					printf("STOP                 - Stop execution\n");
					printf("SUB name(args)       - Define a subroutine\n");
					printf("SWAP var1, var2      - Swap two variables\n");
					printf("TEXTMODE n           - Set text mode\n");
					printf("WEND                 - End of WHILE loop\n");
					printf("WHILE condition      - WHILE loop\n");
					printf("\n");
                    color(11, BGCOLOR);
					printf("OPERATORS\n");
                    color(FGCOLOR, BGCOLOR);
					printf("---------------------------------------\n");
					printf("+  -  *  /  ^        - Arithmetic\n");
					printf("MOD                  - Modulo (remainder)\n");
					printf("AND  OR  XOR  NOT    - Logical/Bitwise\n");
					printf("=  <>  <  >  <=  >=  - Comparison\n");
					printf("+=  -=  *=  /=       - Compound assignment\n");
					printf("\n");
                    color(11, BGCOLOR);
					printf("NUMERIC FUNCTIONS\n");
                    color(FGCOLOR, BGCOLOR);
					printf("---------------------------------------\n");
					printf("ABS(n)    ATN(n)    COS(n)    EXP(n)    INT(n)\n");
					printf("LOG(n)    LOG10(n)  RND       ROUND(n)  SGN(n)\n");
					printf("SIN(n)    SQR(n)    SQRT(n)   TAN(n)    TIMER\n");
					printf("VAL(s$)   UBOUND(arr [,dim])\n");
					printf("\n");
                    color(11, BGCOLOR);
					printf("STRING FUNCTIONS\n");
                    color(FGCOLOR, BGCOLOR);
					printf("---------------------------------------\n");
					printf("ASC(s$)        CHR$(n)        CRLF$          ENC$(s$)\n");
					printf("ENVIRON$(s$)   EXTRACT$(s,d)  FINDFIRST$(p)  FINDNEXT$\n");
					printf("HEX$(n)        INSTR(s$,f$)   INPUTBOX$(p$)  IREPLACE$(s,o,n)\n");
					printf("LCASE$(s$)     LEFT$(s$,n)    LEN(s$)        LTRIM$(s$)\n");
					printf("MCASE$(s$)     MID$(s$,p,n)   REMAIN$(s$,d$) REMOVE$(s$,c$)\n");
					printf("REPEAT$(s$,n)  RETAIN$(s$,c$) RIGHT$(s$,n)   RTRIM$(s$)\n");
					printf("SPACE$(n)      STR$(n)        TRIM$(s$)      UCASE$(s$)\n");
					printf("USING$(f$,n)\n");
					printf("\n");
                    color(11, BGCOLOR);
					printf("PATTERN MATCHING\n");
                    color(FGCOLOR, BGCOLOR);
					printf("---------------------------------------\n");
					printf("LIKE(string$, pattern$) - Pattern match (case-insensitive)\n");
					printf("  Wildcards:  *  = match any characters (zero or more)\n");
					printf("              ?  = match any single character\n");
					printf("              #  = match any single digit (0-9)\n");
					printf("  Example: LIKE(\"Hello123\", \"He*#\") returns TRUE\n");
					printf("\n");
                    color(11, BGCOLOR);
					printf("FILE & OTHER FUNCTIONS\n");
                    color(FGCOLOR, BGCOLOR);
					printf("---------------------------------------\n");
					printf("EOF(#n)       LOF(file$)    EXIST(file$)   KEYPRESS\n");
					printf("OK_CANCEL(p$) VERIFY(s$,c$) YN_CANCEL(p$)\n");
					printf("\n");
                    color(11, BGCOLOR);
					printf("SYSTEM VARIABLES / CONSTANTS\n");
                    color(FGCOLOR, BGCOLOR);
					printf("---------------------------------------\n");
					printf("CURDIR$   DATE$     FALSE     INKEY$    PI\n");
					printf("SYSDIR$   TEMPDIR$  TIME$     TRUE      WINDIR$\n");
					printf("\n");
                    color(11, BGCOLOR);
					printf("FILE MODES: INPUT, OUTPUT, APPEND, BINARY\n");
                    color(FGCOLOR, BGCOLOR);
					printf("===============================================================================\n");
					printf("\n");
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

				case tokelseif:
					cmdelseif(&V);
					break;

				case tokselect:
					cmdselect(&V);
					break;

				case tokcase:
					cmdcase(&V);
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


				case tokexit:
					// EXIT FUNCTION, EXIT SUB, EXIT FOR, EXIT WHILE, EXIT DO
					if (V.Token != NULL && V.Token->kind == tokfunction)
					{
						if (function_executing)
						{
							function_exit_flag = TRUE;
							V.Token = NULL; // Stop processing this line
						}
						else
						{
							Abort("EXIT FUNCTION outside of function");
						}
					}
					else if (V.Token != NULL && V.Token->kind == toksub)
					{
						if (function_executing)
						{
							function_exit_flag = TRUE;
							V.Token = NULL; // Stop processing this line
						}
						else
						{
							Abort("EXIT SUB outside of sub");
						}
					}
					else if (V.Token != NULL && V.Token->kind == tokfor)
					{
						// EXIT FOR - find and exit the innermost FOR loop
						looprec* l;
						BOOL found = FALSE;

						// Find the FOR loop on the stack
						while (loopbase != NULL && !found)
						{
							if (loopbase->kind == FORLOOP)
							{
								found = TRUE;
							}
							else
							{
								l = loopbase->next;
								free(loopbase);
								loopbase = l;
							}
						}

						if (!found)
						{
							Abort("EXIT FOR without FOR");
						}
						else
						{
							// Skip to NEXT
							V.Token = V.Token->next; // move past FOR token
							if (!skiploop(tokfor, toknext, &V))
								Abort("FOR without NEXT");
							// Pop the FOR loop
							l = loopbase->next;
							free(loopbase);
							loopbase = l;
						}
					}
					else if (V.Token != NULL && V.Token->kind == tokwhile)
					{
						// EXIT WHILE - find and exit the innermost WHILE loop
						looprec* l;
						BOOL found = FALSE;

						while (loopbase != NULL && !found)
						{
							if (loopbase->kind == WHILELOOP)
							{
								found = TRUE;
							}
							else
							{
								l = loopbase->next;
								free(loopbase);
								loopbase = l;
							}
						}

						if (!found)
						{
							Abort("EXIT WHILE without WHILE");
						}
						else
						{
							// Skip to WEND
							V.Token = V.Token->next;
							if (!skiploop(tokwhile, tokwend, &V))
								Abort("WHILE without WEND");
							l = loopbase->next;
							free(loopbase);
							loopbase = l;
						}
					}
					else if (V.Token != NULL && V.Token->kind == tokdo)
					{
						// EXIT DO - find and exit the innermost DO loop
						looprec* l;
						BOOL found = FALSE;

						while (loopbase != NULL && !found)
						{
							if (loopbase->kind == DOLOOP)
							{
								found = TRUE;
							}
							else
							{
								l = loopbase->next;
								free(loopbase);
								loopbase = l;
							}
						}

						if (!found)
						{
							Abort("EXIT DO without DO");
						}
						else
						{
							// Skip to LOOP
							V.Token = V.Token->next;
							if (!skiploop(tokdo, tokloop, &V))
								Abort("DO without LOOP");
							l = loopbase->next;
							free(loopbase);
							loopbase = l;
						}
					}
					else if (V.Token != NULL && V.Token->kind == tokselect)
					{
						// EXIT SELECT - skip to END SELECT
						long nest = 1;
						V.Token = V.Token->next; // move past SELECT token
						
						while (nest > 0)
						{
							while (V.Token == NULL)
							{
								if (stmtline == NULL || stmtline->next == NULL)
								{
									Abort("EXIT SELECT without END SELECT");
									goto exit_select_done;
								}
								stmtline = stmtline->next;
								V.Token = stmtline->txt;
							}

							if (V.Token->kind == tokselect &&
								V.Token->next != NULL &&
								V.Token->next->kind == tokcase)
							{
								nest++;
							}
							else if (V.Token->kind == tokend &&
								V.Token->next != NULL &&
								V.Token->next->kind == tokselect)
							{
								nest--;
								if (nest == 0)
								{
									V.Token = V.Token->next->next;
									goto exit_select_done;
								}
								V.Token = V.Token->next;
							}
							V.Token = V.Token->next;
						}
						exit_select_done:;
					}
					else
					{
						Abort("EXIT requires FOR, WHILE, DO, SELECT, FUNCTION, or SUB");
					}
					break;

				default:
					Abort("Syntax error!");
					break;
				}
			}

			if (!V.elseflag && !IsEndOfStatement(&V))
		 		checkextra(&V);
             
			stmttok = V.Token;
		} while (V.Token != NULL);

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


	ClearScopes(); // clean up on error

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


//*****************************************************************************
// PushScope - Create a new local variable scope when entering a function/sub
//*****************************************************************************
void PushScope(funcrec* f)
{
	scoperec* s = (scoperec*)
		safe_calloc(1, sizeof(scoperec));
	s->next = current_scope;
	s->locals = NULL;
	s->owner = f;
	current_scope = s;
}



//*****************************************************************************
// FreeLocalVar - Free memory used by a local variable
//*****************************************************************************
void FreeLocalVar(localvarrec* lv)
{
	long i, total;

	if (lv == NULL)
		return;

	// Free the permanent proxy
	if (lv->proxy != NULL)
	{
		free(lv->proxy);
		lv->proxy = NULL;
	}

	if (lv->IsStringVar)
	{
		if (lv->numdims > 0 && lv->sarr != NULL)
		{
			// Free array of strings
			total = 1;
			for (i = 0; i < lv->numdims; i++)
				total *= lv->dims[i];
			for (i = 0; i < total; i++)
			{
				if (lv->sarr[i] != NULL)
					free(lv->sarr[i]);
			}
			free(lv->sarr);
		}
		else if (lv->StringVar != NULL)
		{
			free(lv->StringVar);
		}
	}
	else
	{
		if (lv->numdims > 0 && lv->arr != NULL)
		{
			free(lv->arr);
		}
	}
	free(lv);
}


//*****************************************************************************
// PopScope - Destroy current scope and free all its local variables
// BUT DO NOT free static variables (they persist)
//*****************************************************************************
void PopScope(void)
{
	localvarrec* lv;
	localvarrec* next;
	scoperec* old;

	if (current_scope == NULL)
		return;

	// Free all local variables in this scope
	lv = current_scope->locals;
	while (lv != NULL)
	{
		next = lv->next;
		FreeLocalVar(lv);
		lv = next;
	}
	current_scope->locals = NULL;

	// IMPORTANT: DO NOT free static variables here!
	// They persist between function calls
	// We leave current_scope->statics intact

	// Pop the scope (but static variables remain in memory)
	old = current_scope;
	current_scope = current_scope->next;

	// Note: We don't free old->statics here
	// Static variables will be cleaned up when program ends or CLEAR is called
	free(old);
}




void ClearScopes(void)
{
	while (current_scope != NULL)
	{
		// Clear locals but preserve statics
		localvarrec* lv = current_scope->locals;
		localvarrec* next_lv;

		while (lv != NULL)
		{
			next_lv = lv->next;
			FreeLocalVar(lv);
			lv = next_lv;
		}
		current_scope->locals = NULL;

		// Move to next scope
		scoperec* next_scope = current_scope->next;
		free(current_scope);
		current_scope = next_scope;
	}
}



//*****************************************************************************
// FreeStaticVar - Free memory used by a static variable
//*****************************************************************************
void FreeStaticVar(staticvarrec* sv)
{
	long i, total;

	if (sv == NULL)
		return;

	// Free the permanent proxy
	if (sv->proxy != NULL)
	{
		free(sv->proxy);
		sv->proxy = NULL;
	}

	if (sv->IsStringVar)
	{
		if (sv->numdims > 0 && sv->sarr != NULL)
		{
			// Free array of strings
			total = 1;
			for (i = 0; i < sv->numdims; i++)
				total *= sv->dims[i];
			for (i = 0; i < total; i++)
			{
				if (sv->sarr[i] != NULL)
					free(sv->sarr[i]);
			}
			free(sv->sarr);
		}
		else if (sv->StringVar != NULL)
		{
			free(sv->StringVar);
		}
	}
	else
	{
		if (sv->numdims > 0 && sv->arr != NULL)
		{
			free(sv->arr);
		}
	}
	free(sv);
}


//*****************************************************************************
// ClearStaticVars - Clear all static variables (called by CLEAR or program end)
//*****************************************************************************
void ClearStaticVars(void)
{
	scoperec* s = current_scope;
	while (s != NULL)
	{
		staticvarrec* sv = s->statics;
		staticvarrec* next;

		while (sv != NULL)
		{
			next = sv->next;
			FreeStaticVar(sv);
			sv = next;
		}
		s->statics = NULL;

		s = s->next;
	}
}


//*****************************************************************************
// FindLocalVar - Look for a variable in the current local scope
//*****************************************************************************
localvarrec* FindLocalVar(char* name)
{
	localvarrec* lv;

	if (current_scope == NULL)
		return NULL;

	lv = current_scope->locals;
	while (lv != NULL)
	{
		if (_stricmp(lv->name, name) == 0)
			return lv;
		lv = lv->next;
	}
	return NULL;
}

//*****************************************************************************
// CreateLocalVar - Create a new local variable in the current scope
//*****************************************************************************
localvarrec* CreateLocalVar(char* name, BOOL IsStringVar)
{
	localvarrec* lv;
	varrec* proxy;

	if (current_scope == NULL)
	{
		Abort("LOCAL outside of FUNCTION or SUB");
	}

	// Check if already exists in this scope
	if (FindLocalVar(name) != NULL)
	{
		Abort("LOCAL variable already declared");
	}

	lv = (localvarrec*)safe_calloc(1, sizeof(localvarrec));
	strncpy(lv->name, name, VAR_NAME_LEN);
	lv->name[VAR_NAME_LEN] = '\0';
	lv->IsStringVar = IsStringVar;
	lv->numdims = 0;
	lv->arr = NULL;
	lv->sarr = NULL;

	if (IsStringVar)
	{
		lv->StringVar = NULL;
		lv->AddressOfStringVar = &lv->StringVar;
	}
	else
	{
		lv->DoubleVar = 0.0;
		lv->AddressOfDoubleVar = &lv->DoubleVar;
	}

	// Create a permanent proxy varrec for this local variable
	// This proxy persists for the variable's lifetime and provides stable reference
	proxy = (varrec*)safe_calloc(1, sizeof(varrec));
	strncpy(proxy->name, name, VAR_NAME_LEN);
	proxy->name[VAR_NAME_LEN] = '\0';
	proxy->numdims = 0;
	proxy->IsStringVar = IsStringVar;
	if (IsStringVar)
	{
		proxy->AddressOfStringVar = &lv->StringVar;
	}
	else
	{
		proxy->AddressOfDoubleVar = &lv->DoubleVar;
	}
	lv->proxy = proxy;

	// Add to front of locals list
	lv->next = current_scope->locals;
	current_scope->locals = lv;

	return lv;
}


//*****************************************************************************
// CreateStaticVar - Create or find static variable in function's static list
//*****************************************************************************
staticvarrec* CreateStaticVar(char* name, BOOL IsStringVar)
{
	varrec* proxy;

	if (current_scope == NULL || current_scope->owner == NULL)
	{
		return NULL;
	}

	funcrec* f = current_scope->owner;

	// Look in function's static list
	staticvarrec* sv = f->statics;
	while (sv != NULL)
	{
		if (_stricmp(sv->name, name) == 0)
		{
			return sv; // Found existing static
		}
		sv = sv->next;
	}

	// Create new static
	sv = (staticvarrec*)safe_calloc(1, sizeof(staticvarrec));
	strncpy(sv->name, name, VAR_NAME_LEN);
	sv->name[VAR_NAME_LEN] = '\0';
	sv->next = NULL;
	sv->numdims = 0;
	sv->IsStringVar = IsStringVar;

	if (IsStringVar)
	{
		sv->StringVar = NULL;
		sv->AddressOfStringVar = &sv->StringVar;
		sv->sarr = NULL;
	}
	else
	{
		sv->DoubleVar = 0.0;
		sv->AddressOfDoubleVar = &sv->DoubleVar;
		sv->arr = NULL;
	}

	// Create a permanent proxy varrec for this static variable
	// This proxy persists for the variable's lifetime and provides stable reference
	proxy = (varrec*)safe_calloc(1, sizeof(varrec));
	strncpy(proxy->name, name, VAR_NAME_LEN);
	proxy->name[VAR_NAME_LEN] = '\0';
	proxy->numdims = 0;
	proxy->IsStringVar = IsStringVar;
	if (IsStringVar)
	{
		proxy->AddressOfStringVar = &sv->StringVar;
	}
	else
	{
		proxy->AddressOfDoubleVar = &sv->DoubleVar;
	}
	sv->proxy = proxy;

	// Add to function's static list
	sv->next = f->statics;
	f->statics = sv;

	return sv;
}

//*****************************************************************************
// FindStaticVar - Look in function's static list
//*****************************************************************************
staticvarrec* FindStaticVar(char* name)
{
	if (current_scope == NULL || current_scope->owner == NULL)
	{
		return NULL;
	}

	funcrec* f = current_scope->owner;
	staticvarrec* sv = f->statics;

	while (sv != NULL)
	{
		if (_stricmp(sv->name, name) == 0)
		{
			return sv;
		}
		sv = sv->next;
	}

	return NULL;
}
//*************************
#include "Bcx_Runtime.c"
//*************************
