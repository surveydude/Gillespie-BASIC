$INCLUDE "Darkmode.inc"

GUI "GBBASIC", PIXELS

ENUM
    ID_Terminal = 100
    ID_Btn1
    ID_Btn2
    ID_Btn3
    ID_Btn4
    ID_Btn5
    ID_Btn6
    ID_Btn7
END ENUM

CONST BUTTON_WIDTH  = 100
CONST BUTTON_HEIGHT = 40
CONST BUTTON_MARGIN = 10

' Global Variables
DIM AS HWND Form1, hTerminal, hConsoleWnd
DIM AS HWND hBtn1, hBtn2, hBtn3, hBtn4, hBtn5, hBtn6, hBtn7

DIM AS HANDLE hCmdProcess
DIM AS DWORD dwCmdPid
DIM AS LONG idTimer

MACRO BumpNextButton_Y_Pos =  btnY = btnY + BUTTON_HEIGHT + BUTTON_MARGIN

SUB FORMLOAD
    DIM AS INT formWidth
    DIM AS INT formHeight
    DIM AS INT termWidth
    DIM AS INT termHeight
    DIM AS INT btnX
    DIM AS INT btnY
    DIM AS STARTUPINFO si
    DIM AS PROCESS_INFORMATION procInfo

    formWidth = 1040
    formHeight = 800

    ' Calculate terminal dimensions
    termWidth = formWidth - BUTTON_WIDTH - (BUTTON_MARGIN * 3) - 10
    termHeight = formHeight - (BUTTON_MARGIN * 2) - 40

    ' Create main form
    Form1 = BCX_FORM("GUI Gillespie BASIC", 0, 0, formWidth, formHeight)

    ' Create container for the console window
    hTerminal = BCX_BLACKRECT("", Form1, ID_Terminal, BUTTON_MARGIN, BUTTON_MARGIN, termWidth, termHeight)

    ' Button layout
    btnX = termWidth + (BUTTON_MARGIN * 2)  :   btnY = BUTTON_MARGIN

    hBtn1 = BCX_BUTTON("Cls",  Form1, ID_Btn1, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT) : BumpNextButton_Y_Pos
    hBtn2 = BCX_BUTTON("Load", Form1, ID_Btn2, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT) : BumpNextButton_Y_Pos
    hBtn3 = BCX_BUTTON("Run",  Form1, ID_Btn3, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT) : BumpNextButton_Y_Pos
    hBtn4 = BCX_BUTTON("List", Form1, ID_Btn4, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT) : BumpNextButton_Y_Pos
    hBtn5 = BCX_BUTTON("Help", Form1, ID_Btn5, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT) : BumpNextButton_Y_Pos
    hBtn6 = BCX_BUTTON("Lex",  Form1, ID_Btn6, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT) : BumpNextButton_Y_Pos
    hBtn7 = BCX_BUTTON("Stop", Form1, ID_Btn7, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT) : BumpNextButton_Y_Pos

    '***************************************************************************************
    '                 Launch, Capture, and ReParent our console mode app
    '***************************************************************************************

    ' Initialize StartupInfo
    memset(ADDRESSOF(si), 0, SIZEOF(si))
    si.cb = SIZEOF(si)

    ' Set flags to hide the window at creation
    si.dwFlags = STARTF_USESHOWWINDOW BOR STARTF_USEPOSITION
    si.wShowWindow = SW_HIDE
    si.dwX = -32000   ' Initially position the console window
    si.dwY = -32000   ' well past the right edge of the screen

    '**************************************************************************************
    '                Launch the Gillespie BASIC interpreter (Basic.exe)
    '**************************************************************************************

    IF CreateProcess(NULL, "basic.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &procInfo) THEN
        hCmdProcess = procInfo.hProcess
        dwCmdPid = procInfo.dwProcessId

        ' Allow process time to initialize
        SLEEP(250)

        ' Find and Reparent
        EnumWindows((WNDENUMPROC)ADDRESSOF(EnumWindowsProc), (LPARAM)dwCmdPid)

        IF hConsoleWnd THEN
            DIM AS LONG style
            style = GetWindowLongPtr(hConsoleWnd, GWL_STYLE)

            style = style AND BNOT WS_CAPTION
            style = style AND BNOT WS_THICKFRAME
            style = style BOR WS_CHILD

            SetWindowLongPtr(hConsoleWnd, GWL_STYLE, style)
            SetParent(hConsoleWnd, hTerminal)
            MoveWindow(hConsoleWnd, 0, 0, termWidth, termHeight, TRUE)
            ShowWindow(hConsoleWnd, SW_SHOW)
        END IF

        CloseHandle(procInfo.hThread)
    END IF

    '**************************************************
    ' A few gymnastics needed for the initial display
    '**************************************************

    CENTER(Form1)
    IF hConsoleWnd THEN
        SLEEP(500)
        CALL SendToConsoleWindow("cls" + CHR$(13))
        UpdateWindow(hConsoleWnd)
        SetFocus(hConsoleWnd)
    END IF

    '*******************
    ' Activate Darkmode
    '*******************
    CALL DarkMode_Init
    CALL DarkMode_Add_Form(Form1)

    ' Keep the cursor in the console window
    SetTimer(Form1, idTimer, 750, NULL)

    SHOW(Form1)   ' It's SHOWTIME!
END SUB




BEGIN EVENTS
    SELECT CASE CBMSG
    CASE WM_TIMER
        SetFocus(hConsoleWnd)

    CASE WM_COMMAND
        SELECT CASE CBCTL
        CASE ID_Btn1
            CALL SendToConsoleWindow("cls" + CR$)
            SetFocus(hConsoleWnd)

        CASE ID_Btn2
            CALL SendToConsoleWindow("load" + CR$)
            SetFocus(hConsoleWnd)

        CASE ID_Btn3
            CALL SendToConsoleWindow("run" + CR$)
            SetFocus(hConsoleWnd)

        CASE ID_Btn4
            CALL SendToConsoleWindow("list" + CR$)
            SetFocus(hConsoleWnd)

        CASE ID_Btn5
            CALL SendToConsoleWindow("help" + CR$)
            SetFocus(hConsoleWnd)

        CASE ID_Btn6
            CALL SendToConsoleWindow("lex" + CR$)
            SetFocus(hConsoleWnd)

        CASE ID_Btn7
            CALL SendToConsoleWindow("Stop" + CHR$(27, 13)) ' Escape Key + CR$
            SetFocus(hConsoleWnd)
        END SELECT

    CASE WM_SIZE
        DIM AS INT w
        DIM AS INT h
        DIM AS INT termW
        DIM AS INT termH
        DIM AS INT btnX
        DIM AS INT btnY

        ' Get new dimensions from lParam
        w = LOWORD(CBLPARAM)
        h = HIWORD(CBLPARAM)

        ' 1. Calculate new terminal dimensions
        ' Subtract button width and 3 margins (Left, Middle, Right)
        termW = w - BUTTON_WIDTH - (BUTTON_MARGIN * 3)
        termH = h - (BUTTON_MARGIN * 2)

        ' 2. Resize the container and the reparented console
        MoveWindow(hTerminal, BUTTON_MARGIN, BUTTON_MARGIN, termW, termH, TRUE)
        IF hConsoleWnd THEN
            MoveWindow(hConsoleWnd, 0, 0, termW, termH, TRUE)
        END IF

        ' 3. Reposition buttons to stay anchored to the right edge
        btnX = termW + (BUTTON_MARGIN * 2)
        btnY = BUTTON_MARGIN

        ' Update each button position
        MoveWindow(hBtn1, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE) : BumpNextButton_Y_Pos
        MoveWindow(hBtn2, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE) : BumpNextButton_Y_Pos
        MoveWindow(hBtn3, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE) : BumpNextButton_Y_Pos
        MoveWindow(hBtn4, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE) : BumpNextButton_Y_Pos
        MoveWindow(hBtn5, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE) : BumpNextButton_Y_Pos
        MoveWindow(hBtn6, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE) : BumpNextButton_Y_Pos
        MoveWindow(hBtn7, btnX, btnY, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE) : BumpNextButton_Y_Pos


    CASE WM_CLOSE
        IF hCmdProcess THEN
            TerminateProcess(hCmdProcess, 0)
            CloseHandle(hCmdProcess)
        END IF
    END SELECT
END EVENTS



SUB SendToConsoleWindow(text$)
    DIM AS INT n
    n = LEN(text$)
    FOR i AS LONG = 1 TO n
        ' Post message to the console window's message queue
        PostMessage(hConsoleWnd, WM_CHAR, (WPARAM)ASC(MID$(text$, i, 1)), 0)
    NEXT
END SUB



FUNCTION EnumWindowsProc(hWnd AS HWND, lParam AS LPARAM) AS BOOL
    DIM AS DWORD lpdwProcessId
    DIM AS CHAR classname[256]

    GetWindowThreadProcessId(hWnd, ADDRESSOF(lpdwProcessId))

    IF lpdwProcessId = (DWORD)lParam THEN
        GetClassName(hWnd, classname, 256)
        ' Use lstrcmpi (Win32 API) for comparison
        IF lstrcmpi(classname, "ConsoleWindowClass") = 0 THEN
            hConsoleWnd = hWnd
            FUNCTION = FALSE
        END IF
    END IF
    FUNCTION = TRUE
END FUNCTION
