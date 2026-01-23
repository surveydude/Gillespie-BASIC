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

char* Using(char* szMask, double Num)
{
	int CntDec = 0; // Count the number of "0" to left pad the result
	int Decimals = 0; // Count ALL digits RIGHT of the decimal point
	int Dollar = 0; // Flag to LEFT prepend "$" symbol
	int Percent = 0; // Flag to convert result to percent
	int SN = 0; // Flag to use Sci Notation 
	int SN_Digits = 0; // Count of digits RIGHT of decimal
	int Spaces = 0; // Flag for using spaces padding
	int Zeros = 0; // Flag for using zero padding 

	char* BCX_RetStr = BCX_TmpStr(256);
	char Buf_1[256] = { 0 };
	LPCTSTR p = szMask;
	int len;

	// First pass: analyze the format mask

	while (*p)
	{
		if (*p == 36)
			Dollar++; // $
		if (Dollar > 1)
			Dollar = 1; // Restrict
		if (*p == 37)
			Percent++; // %
		if (Percent > 1)
			Percent = 1; // Restrict
		if (*p == 32)
			Spaces++; // Space

		// Track zero padding before decimal
		if (*p == 48 && !CntDec)
			Zeros++;

		// Count decimal places (spaces, # or 0 after decimal point)
		if ((*p == 32 || *p == 35 || *p == 48) && CntDec)
			Decimals++;

		// Count # symbols after decimal for SN precision
		if (*p == 35 && SN && CntDec)
			SN_Digits++;

		if (*p == 46)
			CntDec = 1; // Decimal point

		if (*p == 94)
			SN++; // ^ is for scientific notation

		p++;
	}

	// Second pass: Assemble the resulting string

	if (SN) // Process SN and return
	{
		char Tmp[64] = { 0 };
		char fmt[16] = { 0 };

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

	if (Percent)
		Num = Num * 100.0;
	// Format the number with specified decimals
	sprintf(Buf_1, "%1.*f", Decimals, Num);

	// Find integer part length
	char* decimal_pos = strchr(Buf_1, '.');
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
	char temp[256] = { 0 };
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
		if (i > number_start && !(temp[i - 1] == '-'))
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
	return (char*)_strupr(strcpy(strtmp, S));
}

//****************************************************************

char* lcase(char* S)
{
	char* strtmp = BCX_TmpStr((int)strlen(S));
	return (char*)_strlwr(strcpy(strtmp, S));
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
			while (isalpha(*++s))
				;
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
	UNREFERENCED_PARAMETER(show);
	UNREFERENCED_PARAMETER(shape);

	CONSOLE_CURSOR_INFO cci;

	cursor.X = (SHORT)(col - 1);
	cursor.Y = (SHORT)(row - 1);
	SetConsoleCursorPosition(hConsole, cursor);

	if (GetConsoleCursorInfo(hConsole, &cci))
	{
		cci.bVisible = gCursorVisible;
		cci.dwSize = 12;
		SetConsoleCursorInfo(hConsole, &cci);
	}
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
	printf("\033[2J\033[H"); // For MrBcx's Terminal Emulator
	fflush(stdout);

	COORD coordScreen = {
	0, 0
	};
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
	CmDlgHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)
		SBProc, (HINSTANCE)NULL, GetCurrentThreadId());
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
	for (; ; )
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
	lpdt = (LPDLGTEMPLATE)
		GlobalLock(hgbl);

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
	lpdit = (LPDLGITEMTEMPLATE)
		lpw;
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
	lpdit = (LPDLGITEMTEMPLATE)
		lpw;
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
	lpdit = (LPDLGITEMTEMPLATE)
		lpw;
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
	lpdit = (LPDLGITEMTEMPLATE)
		lpw;
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
	ret = DialogBoxIndirect(hInst, (LPDLGTEMPLATE)
		hgbl, GetActiveWindow(), (DLGPROC)
		Prompter);
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

	for (; ; )
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

void Setup_Console(void)
{
	HWND hConWnd;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	hConWnd = GetConsoleWindow();
    SetConsoleFont("Consolas", 22);
	ShowWindow(hConWnd, SW_HIDE);
	//TextMode(3000);
	SetConsoleTitle(VERSION);
	//Center(hConWnd);
	ShowWindow(hConWnd, SW_SHOW);
}

//****************************************************************

void TextMode(int Y)
{
	SMALL_RECT sr;
	COORD Coord;
	Coord.X = (SHORT)80; // Buffer columns
	Coord.Y = (SHORT)Y; // Buffer rows
	sr.Top = 1; // screen position
	sr.Left = 1; // screen position
	sr.Right = 80; // screen position
	sr.Bottom = 43; // screen position
	SetConsoleScreenBufferSize(hConsole, Coord);
	SetConsoleWindowInfo(hConsole, TRUE, &sr);
	color(FGCOLOR, BGCOLOR);
	cls();
}

//****************************************************************

void Cursorsh(int showFlag)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ci;

	gCursorVisible = showFlag ? TRUE : FALSE;

	if (GetConsoleCursorInfo(hOut, &ci))
	{
		ci.bVisible = gCursorVisible;
		SetConsoleCursorInfo(hOut, &ci);
	}
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

char *BCX_TempStr(size_t iBytes)
{
  static int StrCnt;
  static char *StrFunc[2048];
  static CRITICAL_SECTION cs;
  static LONG initialized = 0;
  // Thread-safe one-time initialization
  if (InterlockedCompareExchange(&initialized, 1, 0) == 0) {
      InitializeCriticalSection(&cs);
  }
  // Allocating memory before locking reduces lock duration and blocking.
  char *newStr = (char *)calloc(iBytes + 1, sizeof(char));
  if (!newStr) return NULL;
  EnterCriticalSection(&cs);
      StrCnt = (StrCnt + 1) & 2047;
      if (StrFunc[StrCnt]) {
        free(StrFunc[StrCnt]);
      }
      StrFunc[StrCnt] = newStr;
  LeaveCriticalSection(&cs);
  return StrFunc[StrCnt];
}

PWSTR AnsiToWide (LPCSTR AnsiStr, UINT CodePage, ULONG dwFlags, BOOL NullTerm)
{
  int AnsiByteLen;
  if (NullTerm)
  {
    AnsiByteLen=-1;
  }
  else
  {
    AnsiByteLen=(int)strlen(AnsiStr);
  }
  int WideCharLen;
  WideCharLen = MultiByteToWideChar(CodePage, dwFlags, AnsiStr, AnsiByteLen, 0, 0);
  if (WideCharLen == 0) return (PWSTR)BCX_TempStr(2);
  PWSTR WideStr;
  WideStr = (PWSTR)BCX_TempStr(WideCharLen * sizeof(WCHAR));
  MultiByteToWideChar(CodePage, dwFlags, AnsiStr, AnsiByteLen, WideStr, WideCharLen);
  WideStr[WideCharLen] = 0;
  return WideStr;
}


BOOL SetConsoleFont (char* fontName,SHORT fontSize)
{
  HANDLE  hConWin={0};
  CONSOLE_FONT_INFOEX  cfi={0};
  BOOL  retval={0};
  hConWin=GetStdHandle(STD_OUTPUT_HANDLE);
  cfi.cbSize=sizeof(cfi);
  retval=GetCurrentConsoleFontEx(hConWin,FALSE, &cfi);
  if(retval==FALSE ){
      return FALSE;
    }
  cfi.dwFontSize.Y=fontSize;
  cfi.dwFontSize.X=0;
  cfi.FontWeight=FW_NORMAL;
  lstrcpyW(cfi.FaceName,AnsiToWide(fontName));
  retval=SetCurrentConsoleFontEx(hConWin,FALSE, &cfi);
  if(retval==FALSE ){
      return FALSE;
    }
  return TRUE;
}
