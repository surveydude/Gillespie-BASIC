// *************************************************
//      Made with BCX BASIC To C/C++ Translator
//            Version 8.3.2 (01/17/2026)
// *************************************************
//    Translated for compiling with a C Compiler
// *************************************************
#ifdef _MSC_VER
  #ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
  #endif
#endif
 
// Windows API headers
#include <windows.h>    // Core Windows API
#include <windowsx.h>   // Windows API macros and functions
#include <commctrl.h>   // Common Controls
#include <commdlg.h>    // Common Dialogs
#include <direct.h>     // Directory handling
#include <locale.h>     // Locale handling
#include <mmsystem.h>   // Multimedia
#include <oaidl.h>      // OLE Automation
#include <objbase.h>    // Component Object Model (COM)
#include <ocidl.h>      // OLE Control interfaces
#include <ole2.h>       // OLE 2.0 functionality
#include <oleauto.h>    // OLE Automation
#include <olectl.h>     // OLE Controls
#include <richedit.h>   // Rich Edit Control
#include <shellapi.h>   // Shell API
#include <shlobj.h>     // Shell Objects
#include <urlmon.h>     // URL Monikers
#include <wchar.h>      // Wide character support (also in ISO C)
#include <wctype.h>     // Wide character classification (also in ISO C)
#include <tchar.h>      // Unicode/ANSI character mapping
#include <unknwn.h>     // COM base interfaces
#include <wingdi.h>     // GDI
#include <wininet.h>    // Internet
#include <winsock.h>    // Windows Sockets
#include <winuser.h>    // User Interface
 
// ISO C Standard Library headers
#include <ctype.h>      // Character classification
#include <math.h>       // Mathematical functions
#include <setjmp.h>     // Non-local jumps
#include <stdarg.h>     // Variable arguments
#include <stddef.h>     // Common definitions
#include <stdio.h>      // Input/output
#include <stdlib.h>     // General utilities
#include <string.h>     // String handling
#include <time.h>       // Date and time
#include <errno.h>      // Error numbers (also POSIX)
 
// C99 Standard headers
#include <stdbool.h>    // Boolean type
#include <inttypes.h>   // Integer types
 
// POSIX headers
#include <fcntl.h>      // File control options
 
// Windows-specific headers
#include <process.h>    // Process control functions
#include <io.h>         // Low-level I/O (Windows POSIX subset)
#include <conio.h>      // Console I/O (Windows-specific)
 
// *************************************************
//            System Defined Macros
// *************************************************
 
#define BCXSTRSIZE 2048
 
// *************************************************
//             User Defined Macros
// *************************************************
 
#define MAX_FORMS 32
#define MAX_DIALOGS_PER_FORM 32
#define DARKBKCOLOR 0x383838
#define DARKTEXTCOLOR 0xFFD2AA
#define MENUCOLOR DARKBKCOLOR
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#define USEDARKMODE TRUE
#define DARKMODE_FORMDATA_CLASS struct _DARKMODE_FORMDATA*
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 40
#define BUTTON_MARGIN 10
#define BumpNextButton_Y_Pos btnY=btnY+BUTTON_HEIGHT+BUTTON_MARGIN

// *************************************************
//                  Compiler Macros
// *************************************************

#ifndef    VK_0
   #define VK_0  0x30
   #define VK_1  0x31
   #define VK_2  0x32
   #define VK_3  0x33
   #define VK_4  0x34
   #define VK_5  0x35
   #define VK_6  0x36
   #define VK_7  0x37
   #define VK_8  0x38
   #define VK_9  0x39
   #define VK_A  0x41
   #define VK_B  0x42
   #define VK_C  0x43
   #define VK_D  0x44
   #define VK_E  0x45
   #define VK_F  0x46
   #define VK_G  0x47
   #define VK_H  0x48
   #define VK_I  0x49
   #define VK_J  0x4A
   #define VK_K  0x4B
   #define VK_L  0x4C
   #define VK_M  0x4D
   #define VK_N  0x4E
   #define VK_O  0x4F
   #define VK_P  0x50
   #define VK_Q  0x51
   #define VK_R  0x52
   #define VK_S  0x53
   #define VK_T  0x54
   #define VK_U  0x55
   #define VK_V  0x56
   #define VK_W  0x57
   #define VK_X  0x58
   #define VK_Y  0x59
   #define VK_Z  0x5A
#endif 

// *************************************************
//                   Microsoft VC++
// *************************************************

#ifndef DECLSPEC_UUID
  #if(_MSC_VER >= 1100) && defined(__cplusplus)
    #define DECLSPEC_UUID(x)  __declspec(uuid(x))
  #else
    #define DECLSPEC_UUID(x)
  #endif
#endif
#if (_MSC_VER >= 1900)            // earlier versions untested
   #include <intrin.h>
      #ifndef _rdtsc
         #define _rdtsc __rdtsc   // MSVC uses 2 underscores
      #endif
   #pragma warning(disable: 4018) // signed/unsigned mismatch warnings
   #pragma warning(disable: 4100) // unreferenced argument warnings
   #pragma warning(disable: 4459) // local hides global declaration
   #pragma warning(disable: 4267) // conversion from type1 to type2 warnings
   #pragma warning(disable: 4305) // truncation from double to float warnings
   #pragma warning(disable: 4312) // truncation from cast warnings
   #pragma warning(disable: 4459) // hides global declaration warnings
   #pragma warning(disable: 4800) // forcing value to bool warnings
   #pragma warning(disable: 4838) // conversion from type1 to type2 warnings
   #pragma warning(disable: 4244) // conversion from type1 to type2 warnings
   #pragma warning(disable: 4245) // conversion from type1 to type2 warnings
   #pragma warning(disable: 4554) // use parentheses to clarify precedence
   #pragma warning(disable: 4389) // conversion from signed to unsigned warnings
#endif

// *************************************************
//                  GCC and CLANG
// *************************************************

#if defined(__GNUC__) || defined(__clang__)
   #ifndef __BCPLUSPLUS__
      #include <x86intrin.h>
   #endif
   #pragma GCC diagnostic ignored "-Wwrite-strings"
   #pragma GCC diagnostic ignored "-Wunused-parameter"
   #pragma GCC diagnostic ignored "-Wunknown-pragmas"
   #pragma GCC diagnostic ignored "-Wdangling-else"
   #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
   #pragma GCC diagnostic ignored "-Wdeprecated"
   #pragma GCC diagnostic ignored "-Wsizeof-pointer-div"
#endif

// *************************************************
//                  Embarcadero C++
// *************************************************

#if defined(__BCPLUSPLUS__)
      #include<malloc.h>
      #if defined (_clang__)
          #include <mmintrin.h>
      #endif
      #define _kbhit kbhit
      #ifndef _rdtsc
         #define _rdtsc __rdtsc  // Uses 2 underscores
      #endif
#endif

// *************************************************
//                     Lcc-Win32
// *************************************************

#if defined(__LCC__)
    #define _strdup    strdup
    #define _fseeki64  fseek
    #define _ftelli64  ftell
    #define _stricmp   stricmp
    #define _strnicmp  strnicmp
    #define _itoa      itoa
    #define _ltoa      ltoa
    #define  PCTSTR    LPCTSTR
    #include <intrinsics.h>
    #include <malloc.h>  // for _msize
    #if defined(__windows_h__)
       #define COMPILE_MULTIMON_STUBS
       #include <multimon.h>
       #include <iehelper.h>
       #include <exdisp.h>
    #endif
#endif

// *************************************************
//                     Pelles C
// *************************************************

#if defined(__POCC__)
    #include <intrin.h>
    #pragma pack_stack(off)
    #pragma warn(disable: 2006)    // Non-portable conversion int to const char*
    #pragma warn(disable: 2007)    // Non-portable inline code
    #pragma warn(disable: 2027)    // Missing prototype
    #pragma warn(disable: 2073)    // Loss of precision float to int
    #pragma warn(disable: 2115)    // Initialized but not used warnings
    #pragma warn(disable: 2118)    // Unreferenced argument warnings
    #pragma warn(disable: 2130)    // Usually flawed reporting
    #pragma warn(disable: 2134)    // Possible infinite loop
    #pragma warn(disable: 2154)    // Buggy unreachable code warning using sizeof
    #pragma warn(disable: 2197)    // Unsigned long int not a std bit-field type
    #pragma warn(disable: 2215)    // Conversion from type1 to type2 warnings
    #pragma warn(disable: 2218)    // Unreferenced parameter
    #pragma warn(disable: 2230)    // Incomplete struct declarations (vbs support)
    #pragma warn(disable: 2232)    // Unable to check variable arguments
    #pragma warn(disable: 2235)    // Not all control paths return a value
    #pragma warn(disable: 2241)    // Function marked for deprecation
    #pragma warn(disable: 2243)    // Use parentheses to clarify
    #pragma warn(disable: 2248)    // Non-portable use of extension
    #pragma warn(disable: 2250)    // Shadowing variable
    #pragma warn(disable: 2251)    // Types with different signedness
    #pragma warn(disable: 2804)    // Consider changing type to size_t warnings
    #pragma warn(disable: 2805)    // Possible anti-aliasing violation warnings
    #pragma warn(disable: 2808)    // Unsequenced assignment warnings
    #pragma warn(disable: 2810)    // Potential realloc warnings
    #pragma warn(disable: 2812)    // Attempt to read from non-readable location
    #pragma warn(disable: 2813)    // Possible out-of-bounds warning (caused by CAST)
#endif

// *************************************************
// Instruct Linker to Search Object/Import Libraries
// *************************************************

#if !defined(__GNUC__) && !defined(__TINYC__)
   #if !(defined(__BCPLUSPLUS__) && defined(_WIN64))
      #if !defined(__LCC__)
    #pragma comment(lib,"kernel32.lib")
    #pragma comment(lib,"user32.lib")
    #pragma comment(lib,"gdi32.lib")
    #pragma comment(lib,"comctl32.lib")
    #pragma comment(lib,"advapi32.lib")
    #pragma comment(lib,"winspool.lib")
    #pragma comment(lib,"shell32.lib")
    #pragma comment(lib,"msimg32.lib")
    #pragma comment(lib,"ole32.lib")
    #pragma comment(lib,"oleaut32.lib")
    #pragma comment(lib,"uuid.lib")
    #pragma comment(lib,"odbc32.lib")
    #pragma comment(lib,"odbccp32.lib")
    #pragma comment(lib,"winmm.lib")
    #pragma comment(lib,"comdlg32.lib")
    #pragma comment(lib,"imagehlp.lib")
    #pragma comment(lib,"version.lib")
    #pragma comment(lib,"wininet.lib")
    #pragma comment(lib,"urlmon.lib")
  #else
    #pragma lib <winspool.lib>
    #pragma lib <shell32.lib>
    #pragma lib <msimg32.lib>
    #pragma lib <ole32.lib>
    #pragma lib <oleaut32.lib>
    #pragma lib <uuid.lib>
    #pragma lib <odbc32.lib>
    #pragma lib <odbccp32.lib>
    #pragma lib <winmm.lib>
    #pragma lib <imagehlp.lib>
    #pragma lib <version.lib>
    #pragma lib <wininet.lib>
    #pragma lib <urlmon.lib>
     #endif
  #endif
#endif
 
// *************************************************
//           User's GLOBAL Enumerations
// *************************************************

enum
  {
    Default = 0,
    AllowDark,
    ForceDark,
    ForceLight,
    eMax
  };

enum
  {
    ID_Terminal = 100,
    ID_Btn1,
    ID_Btn2,
    ID_Btn3,
    ID_Btn4,
    ID_Btn5,
    ID_Btn6,
    ID_Btn7
  };
 
// *************************************************
//               Standard Prototypes
// *************************************************
 
void    BCX_SetMetric (PCTSTR);
void    BCX_RegWnd (PCTSTR, WNDPROC);
void    BCX_InitGUI (void);
HWND    BCX_Form (PCTSTR, int=0, int=0, int=250, int=150, int=-1, int=0);
HWND    BCX_Button (PCTSTR, HWND, int=0, int=0, int=0, int=0, int=0, int=0, int=-1);
HWND    BCX_BlackRect (PCTSTR, HWND, int, int, int, int, int, int=0, int=0);
void    BCX_Set_Form_Color (HWND, ULONG);
void    Center (HWND, HWND=0, HWND=0);
char*   BCX_TempStr (size_t);
char*   vchr (int,...);
char*   mid (PCTSTR, int, int=-1);
char*   trim (PCTSTR);
void     BCX_Sleep (int);
SIZE*   GetTextSize (PCTSTR, HWND=0, HFONT=0);
char*   join(int, PCTSTR, ...);
HINSTANCE BCX_LoadDll (PCTSTR);
void      BCX_UnloadDll (void);
INT_PTR   BCX_DynaCallB (PCTSTR, PCTSTR, int, ...);
//<---UNICODE AWARE
PWSTR AnsiToWide (PCSTR, UINT=CP_ACP, ULONG=MB_PRECOMPOSED, BOOL=TRUE);
//>---UNICODE AWARE
 
// *************************************************
//          User Defined Types And Unions
// *************************************************

typedef struct _DarkMode_FormData
{
  HWND  hwnd;
  WNDPROC  Darkmode_wndProc;
  HBRUSH  hBrush;
  HBRUSH  hEditBrush;
  HBRUSH  hListBoxBrush;
  int  dialogCount;
  HWND  dialogs[MAX_DIALOGS_PER_FORM];
  WNDPROC  dialogProcs[MAX_DIALOGS_PER_FORM];
}DarkMode_FormData, *LPDARKMODE_FORMDATA, *DARKMODE_FORMDATA_PTR;

typedef INT (WINAPI *TShouldAppsUseDarkMode)();

typedef INT (WINAPI *TAllowDarkModeForWindow)(HWND, INT);

typedef INT (WINAPI *TSetPreferredAppMode)(INT);

typedef INT (WINAPI *TFlushMenuThemes)();

typedef INT (WINAPI *TRefreshImmersiveColorPolicyState)();
 
// *************************************************
//                System Variables
// *************************************************
 
static char    CR  [2]={13,0};    // Carr Rtn
static  HINSTANCE BCX_DllStore[256];

// *************************************************
//            User's Global Variables
// *************************************************

static HMODULE FHandle;
static int gDarkMode_FormCount;
static TFlushMenuThemes FlushMenuThemes;
static TSetPreferredAppMode SetPreferredAppMode;
static TShouldAppsUseDarkMode ShouldAppsUseDarkMode;
static TAllowDarkModeForWindow AllowDarkModeForWindow;
static TRefreshImmersiveColorPolicyState RefreshImmersiveColorPolicyState;
static float BCX_ScaleX;
static float BCX_ScaleY;
static HINSTANCE BCX_hInstance;
static HWND BCX_hwndMDIClient;
static WNDCLASSEX BCX_WndClass;
static BOOL BCX_GUI_Init;
static char    BCX_ClassName[BCXSTRSIZE];
static HWND Form1;
static HWND hTerminal;
static HWND hConsoleWnd;
static HWND hBtn1;
static HWND hBtn2;
static HWND hBtn3;
static HWND hBtn4;
static HWND hBtn5;
static HWND hBtn6;
static HWND hBtn7;
static HANDLE hCmdProcess;
static ULONG dwCmdPid;
static long idTimer;
static HFONT BcxFont;
static DarkMode_FormData gDarkMode_Forms[MAX_FORMS];
 
// *************************************************
//               Standard Macros
// *************************************************
 
#define DefFont ((BcxFont!=0)?BcxFont:(HFONT)SNDMSG(hWnd, WM_GETFONT, 0, 0))
 
HFONT GetDefaultFont(void)
{
    HFONT result = BcxFont;
    if (result == 0) {
        NONCLIENTMETRICS ncm;
        ncm.cbSize = sizeof(NONCLIENTMETRICS);
        if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0)) {
            BcxFont = CreateFontIndirect(&ncm.lfMessageFont);
            result = BcxFont;
        } else {
            result = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
       }
    }
    return result;
}
#define DefaultFont GetDefaultFont()
 
#define CAST(to_type,old_obj) ((to_type)(old_obj))
#define BNOT ~(int)
#define SHOWHWND(Window)RedrawWindow(Window,0,0,0);ShowWindow(Window,SW_SHOW);
 
// *************************************************
//               User's Prototypes
// *************************************************
 
void DarkMode_Init (void);
int DarkMode_FindFormIndex (HWND);
void DarkMode_Add_Form (HWND);
void DarkMode_Add_Dialog (HWND);
void DarkMode_ApplyToChildren (HWND);
int Get_My_Dpi (void);
int DarkMode_ScaleForDPI (int);
LRESULT CALLBACK DarkMode_Form_WndProc (HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK DarkMode_Dialog_WndProc (HWND,UINT,WPARAM,LPARAM);
void DarkMode_ChangeMenuBG (HWND,HMENU,COLORREF);
void FormLoad (void);
LRESULT CALLBACK WndProc (HWND,UINT,WPARAM,LPARAM);
void SendToConsoleWindow (char*);
BOOL EnumWindowsProc (HWND,LPARAM);

// *************************************************
//            User's Global Initialized Arrays
// *************************************************
 
// *************************************************
//                 Runtime Functions
// *************************************************
 
HINSTANCE BCX_LoadDll (PCTSTR DllName)
{
  static int DllCnt;
  static int RegUnload;
  if (!RegUnload) RegUnload =! atexit(BCX_UnloadDll);
  DllCnt = (DllCnt+1) & 255;
  FreeLibrary(BCX_DllStore[DllCnt]);
  return BCX_DllStore[DllCnt] = LoadLibrary(DllName);
}

void BCX_UnloadDll (void)
{
  int ii;
  for(ii = 255;ii >= 0;ii--)
    FreeLibrary(BCX_DllStore[ii]);
}

INT_PTR BCX_DynaCallB(PCTSTR FuncName, PCTSTR DllName, int nArgs, ...)
{
  HINSTANCE hInst = 0;
  FARPROC lpFunc = 0;
  INT_PTR result = 0;
  INT_PTR *argtable = (INT_PTR*)malloc(nArgs * sizeof(*argtable));
  char buff[256];
  va_list ap;
  int i;
  hInst = GetModuleHandle(DllName);
  if (hInst == NULL)
  {
    hInst = BCX_LoadDll(DllName);
  }
  lpFunc = GetProcAddress(hInst, FuncName);
  if (lpFunc == NULL)
  {
    sprintf(buff, "%s%s", FuncName, "A");
    lpFunc = GetProcAddress(hInst, buff);
  }
  if (lpFunc == NULL)
  {
    sprintf(buff, "_%s", FuncName);
    lpFunc = GetProcAddress(hInst, buff);
  }
  if (lpFunc)
  {
    va_start(ap, nArgs);
    for (i = 0; i < nArgs; i++)
    {
      argtable[i] = va_arg(ap, INT_PTR);
    }
    va_end(ap);
 
    // Define our function pointer and calling convention
    typedef INT_PTR (WINAPI *DynamicFunc)(INT_PTR, INT_PTR, INT_PTR, INT_PTR, 
                                          INT_PTR, INT_PTR, INT_PTR, INT_PTR,
                                          INT_PTR, INT_PTR, INT_PTR, INT_PTR,
                                          INT_PTR, INT_PTR, INT_PTR, INT_PTR);
 
    #if defined(__clang__) // Silence Clang & Gcc warning
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
    #elif defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcast-function-type"
    #endif
 
    DynamicFunc func = (DynamicFunc)lpFunc;
 
    #if defined(__clang__)  // Restore Clang & Gcc warning
        #pragma clang diagnostic pop
    #elif defined(__GNUC__)
        #pragma GCC diagnostic pop
    #endif
 
    // Handle the case based on number of arguments
    switch (nArgs) {
        case 0:
            result = func(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            break;
        case 1:
            result = func(argtable[0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            break;
        case 2:
            result = func(argtable[0], argtable[1], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            break;
        case 3:
            result = func(argtable[0], argtable[1], argtable[2], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            break;
        case 4:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            break;
        case 5:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            break;
        case 6:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            break;
        case 7:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], 0, 0, 0, 0, 0, 0, 0, 0, 0);
            break;
        case 8:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], argtable[7], 
                         0, 0, 0, 0, 0, 0, 0, 0);
            break;
        case 9:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], argtable[7], 
                         argtable[8], 0, 0, 0, 0, 0, 0, 0);
            break;
        case 10:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], argtable[7], 
                         argtable[8], argtable[9], 0, 0, 0, 0, 0, 0);
            break;
        case 11:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], argtable[7], 
                         argtable[8], argtable[9], argtable[10], 0, 0, 0, 0, 0);
            break;
        case 12:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], argtable[7], 
                         argtable[8], argtable[9], argtable[10], argtable[11], 
                         0, 0, 0, 0);
            break;
        case 13:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], argtable[7], 
                         argtable[8], argtable[9], argtable[10], argtable[11],
                         argtable[12], 0, 0, 0);
            break;
        case 14:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], argtable[7], 
                         argtable[8], argtable[9], argtable[10], argtable[11],
                         argtable[12], argtable[13], 0, 0);
            break;
        case 15:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], argtable[7], 
                         argtable[8], argtable[9], argtable[10], argtable[11],
                         argtable[12], argtable[13], argtable[14], 0);
            break;
        case 16:
            result = func(argtable[0], argtable[1], argtable[2], argtable[3], 
                         argtable[4], argtable[5], argtable[6], argtable[7], 
                         argtable[8], argtable[9], argtable[10], argtable[11],
                         argtable[12], argtable[13], argtable[14], argtable[15]);
            break;
        default:
            // More than 16 arguments not supported
            break;
    }
  }
  free(argtable);
  return result;
}

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

char *mid (PCTSTR szSrc, int start, int length)
{
  char *strtmp;
  int tmplen = (int)strlen(szSrc);
  if (start > tmplen||start < 1) return BCX_TempStr(1);
  if (length < 0 || length > (tmplen-start) + 1)
    length = (tmplen-start) + 1;
  strtmp = BCX_TempStr(length);
  return (char*)memcpy(strtmp, &szSrc[start - 1], length);
}

char *trim (PCTSTR szSrc)
{
  if (0 == szSrc[0]) return (char*)szSrc;
  while(*szSrc == 32 || (*szSrc >= 9 && *szSrc <= 13)) szSrc++;
  size_t i = strlen(szSrc);
  while(i > 0 && (szSrc[i-1] == 32 || (szSrc[i-1] >= 9 && szSrc[i-1] <= 13))) i--;
  char *strtmp = BCX_TempStr(i);
  return (char*)memcpy(strtmp, szSrc, i);
}

void BCX_Sleep (int Milliseconds)
{
  static UINT Initialized;
  static UINT OriginalPeriod;
  static UINT CallCount;
  // If called with 0, explicitly reset timer resolution and exit
  if(Milliseconds <= 0 || CallCount >=65535){
      if(Initialized){
          timeEndPeriod(OriginalPeriod);
          Initialized = FALSE;
        }
      return;
    }
  // Set timer resolution to 1 ms if not already initialized
  if(!Initialized){
      TIMECAPS  tMinRes;
      timeGetDevCaps(&tMinRes,sizeof(TIMECAPS));
      OriginalPeriod = tMinRes.wPeriodMin;
      timeBeginPeriod(OriginalPeriod); // Typically sets to 1 ms
      Initialized = TRUE;
    }
  Sleep(Milliseconds);
}

char *vchr(int charcount, ...)
{
  int c = 0, ii = charcount;
  char *s_ = BCX_TempStr(charcount + 1);
  va_list marker;
  s_[ii] = 0;
  va_start(marker, charcount);
  while (ii-- > 0) s_[c++] = (char) va_arg(marker, int);
  va_end(marker);
  return s_;
}

char *join(int ArgCount, PCTSTR Str1, ...)
{
  va_list  ap={0};
  PSTR BCX_RetStr = NULL;
  PSTR currentStr = NULL;
  int i;
  size_t totalLen;
  totalLen = strlen(Str1);
                                      //  Calc memory reqmnt
  va_start(ap,Str1);
  for(i=1; i<=ArgCount-1; i++)
    {
      currentStr = va_arg(ap,PSTR);
      if(strlen(currentStr)>0) {
          totalLen += (int)strlen(currentStr);
        }
    }
  va_end(ap);
                                      //  Allocate memory
  BCX_RetStr = BCX_TempStr(totalLen);
  if(BCX_RetStr == 0) {
      return NULL;                    //   Not a good sign :-(
    }
                                      //   Init result with Str1
  strcpy(BCX_RetStr,Str1);
                                      //   Concat remainiing strings
  va_start(ap,Str1);
  for(i=1; i<=ArgCount-1; i++)
    {
      currentStr = va_arg(ap,PSTR);
      if((int)strlen(currentStr)>0) {
          strcat(BCX_RetStr,currentStr);
        }
    }
  va_end(ap);
  return BCX_RetStr;
}

//<---UNICODE AWARE
PWSTR AnsiToWide (PCSTR AnsiStr, UINT CodePage, ULONG dwFlags, BOOL NullTerm)
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
//>---UNICODE AWARE

void Center (HWND hwnd, HWND Xhwnd, HWND Yhwnd)
{
  RECT rect, rectP;
  int x, y, width, height;
  int screenwidth, screenheight;
  HMONITOR    hMonitor={0};
  MONITORINFO MonitorInfo;
  memset(&MonitorInfo, 0, sizeof(MONITORINFO));
 
  GetWindowRect((HWND)hwnd, &rect);
  width = rect.right-rect.left;
  height = rect.bottom-rect.top;
  if(GetSystemMetrics(SM_CMONITORS) == 1)
    {
      hMonitor = MonitorFromRect(&rect,MONITOR_DEFAULTTOPRIMARY);
    }
  else
    {
      hMonitor = MonitorFromRect(&rect,MONITOR_DEFAULTTONEAREST);
    }
  MonitorInfo.cbSize = sizeof(MONITORINFO);
  GetMonitorInfo(hMonitor, &MonitorInfo);
  if(Xhwnd==NULL)
    {
      rect.left = MonitorInfo.rcWork.left + ((MonitorInfo.rcWork.right - MonitorInfo.rcWork.left - width)/2);
      rect.top = MonitorInfo.rcWork.top + ((MonitorInfo.rcWork.bottom - MonitorInfo.rcWork.top - height)/2);
      rect.right = rect.left + width;
      rect.bottom = rect.top + height;
      SetWindowPos(hwnd, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
      return;
    }
  GetWindowRect(hwnd, &rect);
  GetWindowRect(Xhwnd, &rectP);
  width = rect.right-rect.left;
  x = ((rectP.right-rectP.left)-width)/2+rectP.left;
  if(Yhwnd == NULL)
    {
      height = rect.bottom-rect.top;
      y = ((rectP.bottom-rectP.top)-height)/2+rectP.top;
    }
  else
    {
      GetWindowRect(Yhwnd, &rectP);;
      height = rect.bottom-rect.top;;
      y = ((rectP.bottom-rectP.top)-height)/2+rectP.top;
    }
  screenwidth = MonitorInfo.rcWork.right;
  screenheight = MonitorInfo.rcWork.bottom;
  if(x < MonitorInfo.rcWork.left)
    {
      x = MonitorInfo.rcWork.left+(width/2);
    }
  if(y < MonitorInfo.rcWork.top)
    {
      y = MonitorInfo.rcWork.top+(height/2);
    }
  if(((x + width>screenwidth)))
    {
      x = screenwidth-width;
    }
  if(((y + height>screenheight)))
    {
      y = screenheight-height;
    }
  MoveWindow (hwnd, x, y, width, height, FALSE);
}

HWND BCX_Form (PCTSTR Caption, int X, int Y, int W, int H, int Style, int Exstyle)
{
    if (Style == (-1))
        Style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    HWND A = CreateWindowEx(Exstyle, BCX_ClassName, Caption, Style, X * BCX_ScaleX, Y * BCX_ScaleY,
             BCX_ScaleX * (4 + W), BCX_ScaleY * (12 + H), NULL, (HMENU)(UINT_PTR)NULL, BCX_hInstance, NULL);
 
    SendMessage(A, (UINT_PTR) WM_SETFONT, (WPARAM)((BcxFont == 0) ? GetStockObject
               (DEFAULT_GUI_FONT) : BcxFont), (LPARAM) MAKELPARAM(FALSE, 0));
    SetWindowPos(A, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
    return A;
}

HWND BCX_Button(PCTSTR Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
  if (!Style)
    Style = WS_CHILD|WS_VISIBLE|BS_MULTILINE|BS_PUSHBUTTON|WS_TABSTOP;
  if (Exstyle == (-1))
    Exstyle = WS_EX_STATICEDGE;
  HWND A = CreateWindowEx(Exstyle,"button", Text, Style,
                          X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
                          hWnd,(HMENU)(UINT_PTR)id, BCX_hInstance, NULL);
  SendMessage(A, (UINT_PTR)WM_SETFONT, (WPARAM)DefaultFont, (LPARAM)MAKELPARAM(FALSE, 0));
  if (W==0)
  {
    SIZE *size = GetTextSize(Text, A);
    MoveWindow(A, X*BCX_ScaleX, Y*BCX_ScaleY, size->cx+28, size->cy+12, TRUE);
  }
  return A;
}

HWND BCX_BlackRect(PCTSTR Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
  HWND  A;
  if (!Style)
    Style = WS_CHILD|SS_NOTIFY|WS_VISIBLE|SS_BLACKRECT;
  A = CreateWindowEx(Exstyle,"static",NULL,Style,
                     X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
                     hWnd,(HMENU)(UINT_PTR)id, BCX_hInstance, NULL);
  SendMessage(A, (UINT_PTR)WM_SETFONT, (WPARAM)DefaultFont, (LPARAM)MAKELPARAM(FALSE, 0));
  return A;
}

SIZE* GetTextSize (PCTSTR Text, HWND hWnd, HFONT fnt)
{
  HDC  hdc = GetDC(hWnd);
  if (!fnt) fnt = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
  HFONT sobj = (HFONT)SelectObject(hdc, fnt);
  static SIZE sz;
  GetTextExtentPoint32(hdc, (char*)Text, strlen(Text), &sz);
  SelectObject(hdc, sobj);
  ReleaseDC(hWnd, hdc);
  return (&sz);
}

void BCX_Set_Form_Color (HWND hWnd, ULONG Kolor)
{
  HBRUSH hbr = CreateSolidBrush(Kolor);
  DeleteObject((HBRUSH)SetClassLongPtr(hWnd,GCLP_HBRBACKGROUND,(LONG_PTR)hbr));
  InvalidateRect (hWnd,NULL,TRUE);
}

void BCX_SetMetric (PCTSTR metric)
{
  if (!BCX_GUI_Init)
    BCX_InitGUI();
  if (_stricmp(metric,"PIXELS")==0)
  {
    BCX_ScaleX = 1;
    BCX_ScaleY = 1;
  }
  else if (_stricmp((char*)metric,"DPI")==0)
  {
    HDC  hDc = GetDC(0);
    BCX_ScaleX = (GetDeviceCaps(hDc, LOGPIXELSX) / 96);
    BCX_ScaleY = (GetDeviceCaps(hDc, LOGPIXELSY) / 96);
    ReleaseDC(0, hDc);
  }
  else
  {
    RECT  rc = {0,0,4,8};
    MapDialogRect(NULL,&rc);
    BCX_ScaleX = rc.right/2;
    BCX_ScaleY = rc.bottom/4;
  }
}

void BCX_RegWnd (PCTSTR classname, WNDPROC Form_WndProc)
{
  if (classname[0] == 0)
  {
    MessageBox (GetActiveWindow(),
                "Empty String For BCX_ClassName NOT Allowed",
                "Empty ClassName",0);
    fflush(stdout);
    ExitProcess(1);
  }
  if (GetClassInfoEx(BCX_hInstance, classname, &BCX_WndClass)!=0)
  {
    if (!Form_WndProc)
      UnregisterClass(classname,BCX_hInstance);
    return;
  }
  if (!BCX_GUI_Init)
    BCX_InitGUI();
  if (BCX_ScaleX == 0 && BCX_ScaleY == 0)
    BCX_SetMetric("Pixels");
  strcpy(BCX_ClassName,classname);
  BCX_WndClass.lpfnWndProc = Form_WndProc;
  BCX_WndClass.hInstance = BCX_hInstance;
  BCX_WndClass.lpszClassName = classname;
  RegisterClassEx(&BCX_WndClass);
}

void BCX_InitGUI (void)
{
  INITCOMMONCONTROLSEX iccex;
  if (BCX_GUI_Init)
    return;
  BCX_ScaleX = 1;
  BCX_ScaleY = 1;
  BCX_hInstance = GetModuleHandle(NULL);
  BCX_hwndMDIClient = NULL;
  BCX_WndClass.cbSize = sizeof(BCX_WndClass);
  BCX_WndClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC|CS_DBLCLKS;
  BCX_WndClass.cbClsExtra = 0;
  BCX_WndClass.cbWndExtra = 0;
  BCX_WndClass.hIcon =  LoadIcon(NULL,IDI_WINLOGO);;
  BCX_WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  BCX_WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
  BCX_WndClass.lpszMenuName = NULL;
  BCX_WndClass.hIconSm = NULL;
  iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  iccex.dwICC = ICC_LISTVIEW_CLASSES|ICC_TREEVIEW_CLASSES |
                ICC_BAR_CLASSES|ICC_TAB_CLASSES|ICC_UPDOWN_CLASS |
                ICC_PROGRESS_CLASS|ICC_USEREX_CLASSES|ICC_DATE_CLASSES;
  InitCommonControlsEx(&iccex);
  OleInitialize(0);
  CoInitialize(0);
  BCX_GUI_Init = TRUE;
}

// *************************************************
//            User's Subs and Functions
// *************************************************

void DarkMode_Init (void)
{
  gDarkMode_FormCount=0;
  if(FHandle==NULL ){
      FHandle=LoadLibrary("uxtheme.dll");
      if(FHandle ){
          RefreshImmersiveColorPolicyState=(TRefreshImmersiveColorPolicyState)GetProcAddress(FHandle,MAKEINTRESOURCE(104));
          ShouldAppsUseDarkMode=(TShouldAppsUseDarkMode)GetProcAddress(FHandle,MAKEINTRESOURCE(132));
          AllowDarkModeForWindow=(TAllowDarkModeForWindow)GetProcAddress(FHandle,MAKEINTRESOURCE(133));
          SetPreferredAppMode=(TSetPreferredAppMode)GetProcAddress(FHandle,MAKEINTRESOURCE(135));
          FlushMenuThemes=(TFlushMenuThemes)GetProcAddress(FHandle,MAKEINTRESOURCE(136));
        }
    }
}

int DarkMode_FindFormIndex (HWND hwnd)
{
  int  i={0};
  int  foundIndex={0};
  foundIndex=-1;
  for(i=0; i<=gDarkMode_FormCount-1; i++)
    {
      if(gDarkMode_Forms[i].hwnd==hwnd ){
          foundIndex=i;
          goto L1000;
        }
    }
L1000:;
  return foundIndex;
}

void DarkMode_Add_Form (HWND hwnd)
{
  if(gDarkMode_FormCount==0 ){
      DarkMode_Init();
    }
  if(gDarkMode_FormCount>=MAX_FORMS ){
      return;
    }
  if(DarkMode_FindFormIndex(hwnd)!=-1 ){
      return;
    }
  int  formIndex={0};
  formIndex=gDarkMode_FormCount;
  gDarkMode_Forms[formIndex].hwnd=hwnd;
  gDarkMode_Forms[formIndex].dialogCount=0;
  BOOL  b=TRUE;
  BCX_DynaCallB("DwmSetWindowAttribute","Dwmapi.dll",4,hwnd,DWMWA_USE_IMMERSIVE_DARK_MODE, &b,sizeof(BOOL));
  AllowDarkModeForWindow(hwnd,TRUE);
  SetPreferredAppMode(ForceDark);
  RefreshImmersiveColorPolicyState();
  FlushMenuThemes();
  BCX_DynaCallB("SetWindowTheme","uxtheme.dll",3,hwnd,AnsiToWide("Dark_Explorer"),NULL);
  SetPreferredAppMode(2);
  BCX_Set_Form_Color(hwnd,DARKBKCOLOR);
  gDarkMode_Forms[formIndex].hBrush=CreateSolidBrush(DARKBKCOLOR);
  gDarkMode_Forms[formIndex].hEditBrush=CreateSolidBrush(DARKBKCOLOR);
  gDarkMode_Forms[formIndex].hListBoxBrush=CreateSolidBrush(DARKBKCOLOR);
  DarkMode_ApplyToChildren(hwnd);
  gDarkMode_Forms[formIndex].Darkmode_wndProc=SubclassWindow(hwnd,DarkMode_Form_WndProc);
  gDarkMode_FormCount++;
}

void DarkMode_Add_Dialog (HWND hwnd)
{
  int  formIndex={0};
  formIndex=DarkMode_FindFormIndex(GetParent(hwnd));
  if(formIndex==-1 ){
      formIndex=0;
    }
  if(formIndex==-1||gDarkMode_Forms[formIndex].dialogCount>=MAX_DIALOGS_PER_FORM ){
      return;
    }
  BOOL  b=TRUE;
  int  dlgIndex={0};
  dlgIndex=gDarkMode_Forms[formIndex].dialogCount;
  BCX_DynaCallB("DwmSetWindowAttribute","Dwmapi.dll",4,hwnd,DWMWA_USE_IMMERSIVE_DARK_MODE, &b,sizeof(BOOL));
  BCX_Set_Form_Color(hwnd,DARKBKCOLOR);
  DarkMode_ApplyToChildren(hwnd);
  gDarkMode_Forms[formIndex].dialogs[dlgIndex]=hwnd;
  gDarkMode_Forms[formIndex].dialogProcs[dlgIndex]=SubclassWindow(hwnd,DarkMode_Dialog_WndProc);
  gDarkMode_Forms[formIndex].dialogCount++;
}

void DarkMode_ApplyToChildren (HWND obj)
{
  char ClassName[BCXSTRSIZE]={0};
  char ThemeClass[BCXSTRSIZE]={0};
  HWND  child={0};
  child=GetWindow(obj,GW_CHILD);
  while(child!=NULL){
      GetClassName(child,ClassName,BCXSTRSIZE);
      if(strcmp(trim(ClassName),"msctls_progress32")==0){
          BCX_DynaCallB("SetWindowTheme","uxtheme.dll",3,child,AnsiToWide("Explorer"),NULL);
          child=GetWindow(child,GW_HWNDNEXT);
          continue;
          goto L1002;
        }
      if(strcmp(trim(ClassName),"Button")==0){
          ULONG  dwStyle={0};
          ULONG  buttonType={0};
          dwStyle=GetWindowLongPtr(child,GWL_STYLE);
          buttonType=dwStyle&BS_TYPEMASK;
          if(buttonType==BS_RADIOBUTTON||buttonType==BS_AUTORADIOBUTTON||buttonType==BS_CHECKBOX||buttonType==BS_AUTOCHECKBOX||buttonType==BS_GROUPBOX ){
              BCX_DynaCallB("SetWindowTheme","uxtheme.dll",3,child,AnsiToWide(""),AnsiToWide(""));
              InvalidateRect(child,NULL,TRUE);
            }
          else
            {
              BCX_DynaCallB("SetWindowTheme","uxtheme.dll",3,child,AnsiToWide("Explorer"),NULL);
              AllowDarkModeForWindow(child,TRUE);
              SendMessage(child,(UINT)WM_THEMECHANGED,(WPARAM)0,(LPARAM)0);
            }
          goto L1002;
        }
      if(strcmp(trim(ClassName),"static")==0  || strcmp(trim(ClassName),"Listbox")==0){
          BCX_DynaCallB("SetWindowTheme","uxtheme.dll",3,child,AnsiToWide("Dark_Explorer"),NULL);
          goto L1002;
        }
      if(strcmp(trim(ClassName),"SysListView32")==0){
          HWND  hHeader={0};
          hHeader=ListView_GetHeader(child);
          strcpy(ThemeClass,"Dark_Explorer");
          BCX_DynaCallB("SetWindowTheme","uxtheme.dll",3,hHeader,AnsiToWide("Dark_Explorer"),NULL);
          AllowDarkModeForWindow(hHeader,TRUE);
          ListView_SetTextColor(child,DARKTEXTCOLOR);
          ListView_SetBkColor(child,DARKBKCOLOR);
          ListView_SetExtendedListViewStyle(child,LVS_EX_TRANSPARENTBKGND);
          goto L1002;
        }
      if(strcmp(trim(ClassName),STATUSCLASSNAME)==0){
          BCX_DynaCallB("SetWindowTheme","uxtheme.dll",3,child,AnsiToWide(""),AnsiToWide(""));
          SendMessage(child,(UINT)SB_SETBKCOLOR,(WPARAM)0,(LPARAM)DARKBKCOLOR);
          InvalidateRect(child,NULL,TRUE);
          SendMessage(child,(UINT)WM_THEMECHANGED,(WPARAM)0,(LPARAM)0);
          RedrawWindow(child,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
          child=GetWindow(child,GW_HWNDNEXT);
          continue;
        }
      else
        {
          strcpy(ThemeClass,"Explorer");
        }
L1002:;
      if(strcmp(trim(ClassName),"Button")!=0  && strcmp(trim(ClassName),STATUSCLASSNAME)!=0){
          BCX_DynaCallB("SetWindowTheme","uxtheme.dll",3,child,AnsiToWide(ThemeClass),NULL);
          AllowDarkModeForWindow(child,TRUE);
          SendMessage(child,(UINT)WM_THEMECHANGED,(WPARAM)0,(LPARAM)0);
        }
      child=GetWindow(child,GW_HWNDNEXT);
    }
}

int Get_My_Dpi (void)
{
  HWND  This_hwnd={0};
  This_hwnd=GetActiveWindow();
  int  iDpi={0};
  iDpi=GetDpiForWindow(This_hwnd);
  if(iDpi==0 ){
      HDC  hDC={0};
      hDC=GetDC(This_hwnd);
      iDpi=GetDeviceCaps(hDC,LOGPIXELSY);
      ReleaseDC(This_hwnd,hDC);
    }
  return iDpi;
}

int DarkMode_ScaleForDPI (int value)
{
  return (value*Get_My_Dpi()/96);
}

LRESULT CALLBACK DarkMode_Form_WndProc (HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
  int  formIndex={0};
  formIndex=DarkMode_FindFormIndex(hWnd);
  if(formIndex==-1 ){
      return 0;
    }
  if(Msg==WM_CTLCOLOREDIT||Msg==WM_CTLCOLORSTATIC||Msg==WM_CTLCOLORDLG||Msg==WM_CTLCOLORLISTBOX||Msg==WM_CTLCOLORSCROLLBAR ){
      SetBkColor((HDC)wParam,DARKBKCOLOR);
      SetTextColor((HDC)wParam,DARKTEXTCOLOR);
      return (LONGLONG)gDarkMode_Forms[formIndex].hBrush;
    }
  if(Msg==WM_MEASUREITEM ){
      LPMEASUREITEMSTRUCT  lpMIS={0};
      lpMIS= CAST (LPMEASUREITEMSTRUCT,lParam);
      if(lpMIS->CtlType==ODT_MENU ){
          lpMIS->itemHeight=DarkMode_ScaleForDPI(25);
          lpMIS->itemWidth=DarkMode_ScaleForDPI(40);
        }
      else
        {
          PSTR  szTmp={0};
          szTmp=(PSTR)(lpMIS->itemData);
          SIZE  strSize={0};
          HDC  hDC={0};
          hDC=GetDC(hWnd);
          GetTextExtentPoint32(hDC,szTmp,(int)strlen(szTmp), &strSize);
          lpMIS->itemWidth=strSize.cx;
          lpMIS->itemHeight=30;
          ReleaseDC(hWnd,hDC);
        }
      return TRUE;
    }
  if(Msg==WM_DRAWITEM ){
      LPDRAWITEMSTRUCT  itemStruct={0};
      itemStruct=(LPDRAWITEMSTRUCT)lParam;
      HDC  hDC={0};
      hDC=itemStruct->hDC;
      SetTextColor(hDC,DARKTEXTCOLOR);
      SetBkMode(hDC,TRANSPARENT);
      if(itemStruct->CtlType==ODT_MENU ){
          DrawText(hDC,(PSTR)(itemStruct->itemData),-1, &(itemStruct->rcItem),DT_VCENTER|DT_SINGLELINE);
        }
      else
        {
          itemStruct->rcItem.left+=6;
          SetBkColor(hDC,DARKBKCOLOR);
          SetTextColor(hDC,DARKTEXTCOLOR);
          HBRUSH  hBrush={0};
          hBrush=CreateSolidBrush(DARKBKCOLOR);
          FillRect(hDC, &itemStruct->rcItem,hBrush);
          DeleteObject(hBrush);
          DrawText(hDC,(PSTR)(itemStruct->itemData),-1, &itemStruct->rcItem,DT_VCENTER|DT_SINGLELINE);
        }
      return TRUE;
    }
  if(Msg==WM_DESTROY ){
      if(gDarkMode_Forms[formIndex].hBrush!=NULL ){
          DeleteObject(gDarkMode_Forms[formIndex].hBrush);
          gDarkMode_Forms[formIndex].hBrush=NULL;
        }
      if(gDarkMode_Forms[formIndex].hEditBrush!=NULL ){
          DeleteObject(gDarkMode_Forms[formIndex].hEditBrush);
          gDarkMode_Forms[formIndex].hEditBrush=NULL;
        }
      if(gDarkMode_Forms[formIndex].hListBoxBrush!=NULL ){
          DeleteObject(gDarkMode_Forms[formIndex].hListBoxBrush);
          gDarkMode_Forms[formIndex].hListBoxBrush=NULL;
        }
      return CallWindowProc((WNDPROC)gDarkMode_Forms[formIndex].Darkmode_wndProc,hWnd,Msg,wParam,lParam);
    }
  return CallWindowProc((WNDPROC)gDarkMode_Forms[formIndex].Darkmode_wndProc,hWnd,Msg,wParam,lParam);
}

LRESULT CALLBACK DarkMode_Dialog_WndProc (HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
  int  formIndex={0};
  int  dlgIndex={0};
  int  i={0};
  formIndex=-1;
  dlgIndex=-1;
  for(i=0; i<=gDarkMode_FormCount-1; i++)
    {
      int  j={0};
      for(j=0; j<=gDarkMode_Forms[i].dialogCount-1; j++)
        {
          if(gDarkMode_Forms[i].dialogs[j]==hWnd ){
              formIndex=i;
              dlgIndex=j;
              goto L1006;
            }
        }
L1006:;
      if(formIndex!=-1 ){
          goto L1005;
        }
    }
L1005:;
  if(formIndex==-1 ){
      return 0;
    }
  if(Msg==WM_CTLCOLOREDIT||Msg==WM_CTLCOLORSTATIC||Msg==WM_CTLCOLORDLG||Msg==WM_CTLCOLORLISTBOX||Msg==WM_CTLCOLORSCROLLBAR ){
      SetBkColor((HDC)wParam,DARKBKCOLOR);
      SetTextColor((HDC)wParam,DARKTEXTCOLOR);
      return (LONGLONG)gDarkMode_Forms[formIndex].hBrush;
    }
  if(Msg==WM_MEASUREITEM ){
      LPMEASUREITEMSTRUCT  lpMIS={0};
      lpMIS= CAST (LPMEASUREITEMSTRUCT,lParam);
      if(lpMIS->CtlType==ODT_MENU ){
          lpMIS->itemHeight=DarkMode_ScaleForDPI(25);
          lpMIS->itemWidth=DarkMode_ScaleForDPI(40);
        }
      return TRUE;
    }
  if(Msg==WM_DRAWITEM ){
      LPDRAWITEMSTRUCT  itemStruct={0};
      itemStruct=(LPDRAWITEMSTRUCT)lParam;
      HDC  hDC={0};
      hDC=itemStruct->hDC;
      SetTextColor(hDC,DARKTEXTCOLOR);
      SetBkMode(hDC,TRANSPARENT);
      SetBkColor(hDC,DARKBKCOLOR);
      HBRUSH  hBrush={0};
      hBrush=CreateSolidBrush(DARKBKCOLOR);
      FillRect(hDC, &itemStruct->rcItem,hBrush);
      DeleteObject(hBrush);
      DrawText(hDC,(PSTR)(itemStruct->itemData),-1, &itemStruct->rcItem,DT_VCENTER|DT_SINGLELINE);
      return TRUE;
    }
  return CallWindowProc((WNDPROC)gDarkMode_Forms[formIndex].dialogProcs[dlgIndex],hWnd,Msg,wParam,lParam);
}

void DarkMode_ChangeMenuBG (HWND hwnd,HMENU hMenu,COLORREF NewRGB)
{
  MENUINFO  mnuInfo={0};
  mnuInfo.cbSize=sizeof(mnuInfo);
  mnuInfo.fMask=MIM_BACKGROUND|MIM_APPLYTOSUBMENUS;
  mnuInfo.hbrBack=CreateSolidBrush(NewRGB);
  SetMenuInfo(hMenu, &mnuInfo);
}

void FormLoad (void)
{
  int  formWidth={0};
  int  formHeight={0};
  int  termWidth={0};
  int  termHeight={0};
  int  btnX={0};
  int  btnY={0};
  STARTUPINFO  si={0};
  PROCESS_INFORMATION  procInfo={0};
  formWidth=1040;
  formHeight=800;
  termWidth=formWidth-BUTTON_WIDTH-(BUTTON_MARGIN*3)-10;
  termHeight=formHeight-(BUTTON_MARGIN*2)-40;
  Form1=BCX_Form("GUI Gillespie BASIC",0,0,formWidth,formHeight);
  hTerminal=BCX_BlackRect("",Form1,ID_Terminal,BUTTON_MARGIN,BUTTON_MARGIN,termWidth,termHeight);
  btnX=termWidth+(BUTTON_MARGIN*2);
  btnY=BUTTON_MARGIN;
  hBtn1=BCX_Button("Cls",Form1,ID_Btn1,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT);
  BumpNextButton_Y_Pos;
  hBtn2=BCX_Button("Load",Form1,ID_Btn2,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT);
  BumpNextButton_Y_Pos;
  hBtn3=BCX_Button("Run",Form1,ID_Btn3,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT);
  BumpNextButton_Y_Pos;
  hBtn4=BCX_Button("List",Form1,ID_Btn4,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT);
  BumpNextButton_Y_Pos;
  hBtn5=BCX_Button("Help",Form1,ID_Btn5,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT);
  BumpNextButton_Y_Pos;
  hBtn6=BCX_Button("Lex",Form1,ID_Btn6,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT);
  BumpNextButton_Y_Pos;
  hBtn7=BCX_Button("Stop",Form1,ID_Btn7,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT);
  BumpNextButton_Y_Pos;
  memset(&(si),0,sizeof(si));
  si.cb=sizeof(si);
  si.dwFlags=STARTF_USESHOWWINDOW|STARTF_USEPOSITION;
  si.wShowWindow=SW_HIDE;
  si.dwX=-32000;
  si.dwY=-32000;
  if(CreateProcess(NULL,"basic.exe",NULL,NULL,FALSE,0,NULL,NULL, &si, &procInfo)){
      hCmdProcess=procInfo.hProcess;
      dwCmdPid=procInfo.dwProcessId;
      BCX_Sleep(250);
      EnumWindows((WNDENUMPROC)&(EnumWindowsProc),(LPARAM)dwCmdPid);
      if(hConsoleWnd ){
          long  style={0};
          style=GetWindowLongPtr(hConsoleWnd,GWL_STYLE);
          style=style& BNOT WS_CAPTION;
          style=style& BNOT WS_THICKFRAME;
          style=style|WS_CHILD;
          SetWindowLongPtr(hConsoleWnd,GWL_STYLE,style);
          SetParent(hConsoleWnd,hTerminal);
          MoveWindow(hConsoleWnd,0,0,termWidth,termHeight,TRUE);
          ShowWindow(hConsoleWnd,SW_SHOW);
        }
      CloseHandle(procInfo.hThread);
    }
  Center(Form1);
  if(hConsoleWnd ){
      BCX_Sleep(500);
      SendToConsoleWindow(join(2,"cls",vchr(1,13)));
      UpdateWindow(hConsoleWnd);
      SetFocus(hConsoleWnd);
    }
  DarkMode_Init();
  DarkMode_Add_Form(Form1);
  SetTimer(Form1,idTimer,750,NULL);
  SHOWHWND(Form1);
}

LRESULT CALLBACK WndProc (HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
  int  Bcx_RetVal={0};
  if(Msg==WM_TIMER ){
      SetFocus(hConsoleWnd);
      goto L1008;
    }
  if(Msg==WM_COMMAND ){
      if(LOWORD(wParam)==ID_Btn1 ){
          SendToConsoleWindow(join(2,"cls",CR));
          SetFocus(hConsoleWnd);
          goto L1009;
        }
      if(LOWORD(wParam)==ID_Btn2 ){
          SendToConsoleWindow(join(2,"load",CR));
          SetFocus(hConsoleWnd);
          goto L1009;
        }
      if(LOWORD(wParam)==ID_Btn3 ){
          SendToConsoleWindow(join(2,"run",CR));
          SetFocus(hConsoleWnd);
          goto L1009;
        }
      if(LOWORD(wParam)==ID_Btn4 ){
          SendToConsoleWindow(join(2,"list",CR));
          SetFocus(hConsoleWnd);
          goto L1009;
        }
      if(LOWORD(wParam)==ID_Btn5 ){
          SendToConsoleWindow(join(2,"help",CR));
          SetFocus(hConsoleWnd);
          goto L1009;
        }
      if(LOWORD(wParam)==ID_Btn6 ){
          SendToConsoleWindow(join(2,"lex",CR));
          SetFocus(hConsoleWnd);
          goto L1009;
        }
      if(LOWORD(wParam)==ID_Btn7 ){
          SendToConsoleWindow(join(2,"Stop",vchr(2,27,13)));
          SetFocus(hConsoleWnd);
        }
L1009:;
      goto L1008;
    }
  if(Msg==WM_SIZE ){
      int  w={0};
      int  h={0};
      int  termW={0};
      int  termH={0};
      int  btnX={0};
      int  btnY={0};
      w=LOWORD(lParam);
      h=HIWORD(lParam);
      termW=w-BUTTON_WIDTH-(BUTTON_MARGIN*3);
      termH=h-(BUTTON_MARGIN*2);
      MoveWindow(hTerminal,BUTTON_MARGIN,BUTTON_MARGIN,termW,termH,TRUE);
      if(hConsoleWnd ){
          MoveWindow(hConsoleWnd,0,0,termW,termH,TRUE);
        }
      btnX=termW+(BUTTON_MARGIN*2);
      btnY=BUTTON_MARGIN;
      MoveWindow(hBtn1,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT,TRUE);
      BumpNextButton_Y_Pos;
      MoveWindow(hBtn2,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT,TRUE);
      BumpNextButton_Y_Pos;
      MoveWindow(hBtn3,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT,TRUE);
      BumpNextButton_Y_Pos;
      MoveWindow(hBtn4,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT,TRUE);
      BumpNextButton_Y_Pos;
      MoveWindow(hBtn5,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT,TRUE);
      BumpNextButton_Y_Pos;
      MoveWindow(hBtn6,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT,TRUE);
      BumpNextButton_Y_Pos;
      MoveWindow(hBtn7,btnX,btnY,BUTTON_WIDTH,BUTTON_HEIGHT,TRUE);
      BumpNextButton_Y_Pos;
      goto L1008;
    }
  if(Msg==WM_CLOSE ){
      if(hCmdProcess ){
          TerminateProcess(hCmdProcess,0);
          CloseHandle(hCmdProcess);
        }
    }
L1008:;
  if(Msg==WM_DESTROY)
    {
       OleUninitialize();
       CoUninitialize();
       PostQuitMessage(0);
       return EXIT_SUCCESS;
    }
Bcx_RetVal = DefWindowProc(hWnd,Msg,wParam,lParam);
return Bcx_RetVal; // endevents
}

void SendToConsoleWindow (char* text)
{
  int  n={0};
  n=(int)strlen(text);
  for(long i=1; i<=n; i++)
    {
      PostMessage(hConsoleWnd,WM_CHAR,(WPARAM)(UCHAR)*(mid(text,i,1)),0);
    }
}

BOOL EnumWindowsProc (HWND hWnd,LPARAM lParam)
{
  ULONG  lpdwProcessId={0};
  char  classname[256]={0};
  GetWindowThreadProcessId(hWnd,&(lpdwProcessId));
  if(lpdwProcessId==(ULONG)lParam ){
      GetClassName(hWnd,classname,256);
      if(lstrcmpi(classname,"ConsoleWindowClass")==0 ){
          hConsoleWnd=hWnd;
          return FALSE;
        }
    }
  return TRUE;
}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,PSTR CmdLine,int CmdShow) {
    MSG  Msg;
    strcpy(BCX_ClassName,"GBBASIC");
    BCX_SetMetric("pixels");
    BCX_InitGUI();
    BCX_hInstance       =  hInst;
    BCX_WndClass.hIcon  =  LoadIcon(NULL,IDI_WINLOGO);
    BCX_RegWnd(BCX_ClassName, WndProc);
  
    // ******************************************
                      FormLoad();
    // ******************************************
   while(GetMessage(&Msg,NULL,0,0))
     {
      HWND hActiveWindow = GetActiveWindow();
      if((!IsWindow(hActiveWindow)) || (!IsDialogMessage(hActiveWindow,&Msg)))
        {
          TranslateMessage(&Msg);
          DispatchMessage(&Msg);
        }
      }
   return Msg.wParam;
  }
