'*******************************************************************************
' Enhanced Dark Mode Library for BCX BASIC - Supports Multiple Forms & Dialogs
' Version 2.1                                                 October 29, 2025
' Based on BCX code by Armando Rivera, Kevin Diggins, and Jeff Shollenberger
'*******************************************************************************
' USAGE:
'        CALL DarkMode_Init()     ' This is a one-time initialization
'
'        Call DarkMode_Add_Form   (hWnd) for EACH BCX Form
'        Call DarkMode_Add_Dialog (hWnd) for EACH Dynamic Dialog
'*******************************************************************************
$GENFREE
'********************************************************************************
MACRO MAX_FORMS = 32               '  User Configurable
MACRO MAX_DIALOGS_PER_FORM = 32    '  User Configurable
'********************************************************************************
'     Dark Mode Colors
'********************************************************************************
MACRO DARKBKCOLOR   = 0x383838
MACRO DARKTEXTCOLOR = 0xFFD2AA
MACRO MENUCOLOR     = DARKBKCOLOR
MACRO DWMWA_USE_IMMERSIVE_DARK_MODE = 20
MACRO USEDARKMODE = TRUE

ENUM
    Default = 0
    AllowDark
    ForceDark
    ForceLight
    eMax
END ENUM

'------------------
' Type Definitions
'------------------
TYPE DarkMode_FormData
    hwnd AS HWND
    Darkmode_wndProc AS WNDPROC
    hBrush AS HBRUSH
    hEditBrush AS HBRUSH
    hListBoxBrush AS HBRUSH
    dialogCount AS INT
    dialogs[MAX_DIALOGS_PER_FORM] AS HWND
    dialogProcs[MAX_DIALOGS_PER_FORM] AS WNDPROC
END TYPE

'-------------------------
' Global Dark Mode State
'-------------------------
GLOBAL FHandle AS HMODULE
GLOBAL gDarkMode_FormCount AS INT



SUB DarkMode_Init
    '--------------------------------------------------------------------
    ' Function Pointers MUST be used in a SUB or FUNCTION in a GUI app
    '--------------------------------------------------------------------
    $TYPEDEF INT (WINAPI *TShouldAppsUseDarkMode)()
    $TYPEDEF INT (WINAPI *TAllowDarkModeForWindow)(HWND, INT)
    $TYPEDEF INT (WINAPI *TSetPreferredAppMode)(INT)
    $TYPEDEF INT (WINAPI *TFlushMenuThemes)()
    $TYPEDEF INT (WINAPI *TRefreshImmersiveColorPolicyState)()

    GLOBAL FlushMenuThemes                  AS TFlushMenuThemes
    GLOBAL gDarkMode_Forms[MAX_FORMS]       AS DarkMode_FormData
    GLOBAL SetPreferredAppMode              AS TSetPreferredAppMode
    GLOBAL ShouldAppsUseDarkMode            AS TShouldAppsUseDarkMode
    GLOBAL AllowDarkModeForWindow           AS TAllowDarkModeForWindow
    GLOBAL RefreshImmersiveColorPolicyState AS TRefreshImmersiveColorPolicyState

    '--------------------------------------------------------------------
    gDarkMode_FormCount = 0

    ' Load uxtheme.dll and get function pointers (one-time initialization)
    IF FHandle = NULL THEN
        FHandle = LOAD_DLL("uxtheme.dll")
        IF FHandle THEN
            RefreshImmersiveColorPolicyState = GetProcAddress(FHandle, MakeIntResource(104))
            ShouldAppsUseDarkMode            = GetProcAddress(FHandle, MakeIntResource(132))
            AllowDarkModeForWindow           = GetProcAddress(FHandle, MakeIntResource(133))
            SetPreferredAppMode              = GetProcAddress(FHandle, MakeIntResource(135))
            FlushMenuThemes                  = GetProcAddress(FHandle, MakeIntResource(136))
        END IF
    END IF
END SUB


FUNCTION DarkMode_FindFormIndex(hwnd AS HWND) AS INT
    DIM AS INT i, foundIndex
    foundIndex = -1

    FOR i = 0 TO gDarkMode_FormCount - 1
        IF gDarkMode_Forms[i].hwnd = hwnd THEN
            foundIndex = i
            EXIT FOR
        END IF
    NEXT

    FUNCTION = foundIndex
END FUNCTION


SUB DarkMode_Add_Form(hwnd AS HWND)
    IF gDarkMode_FormCount = 0 THEN
        CALL DarkMode_Init()
    END IF

    IF gDarkMode_FormCount >= MAX_FORMS THEN
        EXIT SUB
    END IF

    ' Check if form is already registered
    IF DarkMode_FindFormIndex(hwnd) <> -1 THEN
        EXIT SUB
    END IF

    DIM AS INT formIndex
    formIndex = gDarkMode_FormCount

    ' Initialize form data
    gDarkMode_Forms[formIndex].hwnd = hwnd
    gDarkMode_Forms[formIndex].dialogCount = 0

    DIM AS BOOL b = TRUE

    ' Enable dark mode for TitleBar
    DwmSetWindowAttribute(LIB "Dwmapi.dll", hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &b, SIZEOF(BOOL))

    ' Enable dark mode for MenuItems
    AllowDarkModeForWindow(hwnd, TRUE)
    SetPreferredAppMode(ForceDark)

    RefreshImmersiveColorPolicyState()
    FlushMenuThemes()

    ' Apply dark theme
    SetWindowTheme(LIB "uxtheme.dll", hwnd, UCODE$("Dark_Explorer"), NULL)
    SetPreferredAppMode(2)

    BCX_SET_FORM_COLOR(hwnd, DARKBKCOLOR)

    ' Create brushes for painting
    gDarkMode_Forms[formIndex].hBrush = CreateSolidBrush(DARKBKCOLOR)
    gDarkMode_Forms[formIndex].hEditBrush = CreateSolidBrush(DARKBKCOLOR)
    gDarkMode_Forms[formIndex].hListBoxBrush = CreateSolidBrush(DARKBKCOLOR)

    ' Apply dark mode to all child controls
    CALL DarkMode_ApplyToChildren(hwnd)

    ' Subclass the main window to handle messages
    gDarkMode_Forms[formIndex].Darkmode_wndProc = SubclassWindow(hwnd, DarkMode_Form_WndProc)

    INCR gDarkMode_FormCount
END SUB


SUB DarkMode_Add_Dialog(hwnd AS HWND)
    DIM AS INT formIndex
    formIndex = DarkMode_FindFormIndex(GetParent(hwnd))

    ' If no parent form found, try to find any form (dialog may be owner-drawn)
    IF formIndex = -1 THEN
        formIndex = 0
    END IF

    IF formIndex = -1 OR gDarkMode_Forms[formIndex].dialogCount >= MAX_DIALOGS_PER_FORM THEN
        EXIT SUB
    END IF

    DIM AS BOOL b = TRUE
    DIM AS INT dlgIndex

    dlgIndex = gDarkMode_Forms[formIndex].dialogCount

    ' Enable dark mode for dialog TitleBar
    DwmSetWindowAttribute(LIB "Dwmapi.dll", hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &b, SIZEOF(BOOL))

    ' Set form color
    BCX_SET_FORM_COLOR(hwnd, DARKBKCOLOR)

    ' Apply dark mode to all child controls
    CALL DarkMode_ApplyToChildren(hwnd)

    ' Subclass the dialog to handle messages
    gDarkMode_Forms[formIndex].dialogs[dlgIndex] = hwnd
    gDarkMode_Forms[formIndex].dialogProcs[dlgIndex] = SubclassWindow(hwnd, DarkMode_Dialog_WndProc)

    INCR gDarkMode_Forms[formIndex].dialogCount
END SUB


SUB DarkMode_ApplyToChildren(obj AS HWND)
    DIM ClassName$
    DIM ThemeClass$
    DIM child AS HWND

    child = GetWindow(obj, GW_CHILD)
    DO WHILE child <> NULL
        GetClassName(child, ClassName$, BCXSTRSIZE)

        SELECT CASE TRIM$(ClassName$)

        CASE "msctls_progress32"
            SetWindowTheme(LIB "uxtheme.dll", child, UCODE$("Explorer"), NULL)
            child = GetWindow(child, GW_HWNDNEXT)
            ITERATE

        CASE "Button"
            DIM AS DWORD dwStyle, buttonType
            dwStyle = GetWindowLong(child, GWL_STYLE)
            buttonType  = dwStyle AND BS_TYPEMASK

            SELECT CASE buttonType
            CASE BS_RADIOBUTTON, BS_AUTORADIOBUTTON, BS_CHECKBOX, BS_AUTOCHECKBOX, BS_GROUPBOX
                SetWindowTheme(LIB "uxtheme.dll", child, UCODE$(""), UCODE$(""))
                InvalidateRect(child, NULL, TRUE)
            CASE ELSE
                SetWindowTheme(LIB "uxtheme.dll", child, UCODE$("Explorer"), NULL)
                AllowDarkModeForWindow(child, TRUE)
                SendMessage(child, WM_THEMECHANGED, 0, 0)
            END SELECT

        CASE "static", "Listbox"
            SetWindowTheme(LIB "uxtheme.dll", child, UCODE$("Dark_Explorer"), NULL)

        CASE "SysListView32"
            DIM AS HWND hHeader
            hHeader = ListView_GetHeader(child)
            ThemeClass$ = "Dark_Explorer"
            SetWindowTheme(LIB "uxtheme.dll", hHeader, UCODE$("Dark_Explorer"), NULL)
            AllowDarkModeForWindow(hHeader, TRUE)
            ListView_SetTextColor(child, DARKTEXTCOLOR)
            ListView_SetBkColor(child, DARKBKCOLOR)
            ListView_SetExtendedListViewStyle(child, LVS_EX_TRANSPARENTBKGND)

        CASE STATUSCLASSNAME
            SetWindowTheme(LIB "uxtheme.dll", child, UCODE$(""), UCODE$(""))
            SendMessage(child, SB_SETBKCOLOR, 0, DARKBKCOLOR)
            InvalidateRect(child, NULL, TRUE)
            SendMessage(child, WM_THEMECHANGED, 0, 0)
            RedrawWindow(child, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE)
            child = GetWindow(child, GW_HWNDNEXT)
            ITERATE

        CASE ELSE
            ThemeClass$ = "Explorer"
        END SELECT

        IF TRIM$(ClassName$) <> "Button" AND TRIM$(ClassName$) <> STATUSCLASSNAME THEN
            SetWindowTheme(LIB "uxtheme.dll", child, UCODE$(ThemeClass$), NULL)
            AllowDarkModeForWindow(child, TRUE)
            SendMessage(child, WM_THEMECHANGED, 0, 0)
        END IF

        child = GetWindow(child, GW_HWNDNEXT)
    LOOP
END SUB


FUNCTION Get_My_Dpi() AS INT
    DIM This_hwnd AS HWND
    This_hwnd = GetActiveWindow()
    DIM iDpi AS INT
    iDpi = GetDpiForWindow(This_hwnd)
    IF iDpi = 0 THEN
        DIM hDC AS HDC
        hDC = GetDC(This_hwnd)
        iDpi = GetDeviceCaps(hDC, LOGPIXELSY)
        ReleaseDC(This_hwnd, hDC)
    END IF
    FUNCTION = iDpi
END FUNCTION


FUNCTION DarkMode_ScaleForDPI(value AS INT) AS INT
    FUNCTION = (value * Get_My_Dpi() / 96)
END FUNCTION


CALLBACK FUNCTION DarkMode_Form_WndProc()
    DIM AS INT formIndex
    formIndex = DarkMode_FindFormIndex(CBHWND)

    IF formIndex = -1 THEN
        FUNCTION = 0
    END IF

    SELECT CASE CBMSG

    CASE WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, WM_CTLCOLORDLG, WM_CTLCOLORLISTBOX, WM_CTLCOLORSCROLLBAR
        SetBkColor((HDC)CBWPARAM, DARKBKCOLOR)
        SetTextColor((HDC)CBWPARAM, DARKTEXTCOLOR)
        FUNCTION = (LONGLONG)gDarkMode_Forms[formIndex].hBrush

    CASE WM_MEASUREITEM
        DIM AS LPMEASUREITEMSTRUCT lpMIS
        lpMIS = CAST(LPMEASUREITEMSTRUCT, CBLPARAM)
        IF lpMIS->CtlType = ODT_MENU THEN
            lpMIS->itemHeight = DarkMode_ScaleForDPI(25)
            lpMIS->itemWidth  = DarkMode_ScaleForDPI(40)
        ELSE
            DIM AS LPSTR szTmp
            szTmp = (LPSTR)(lpMIS->itemData)
            DIM AS SIZE strSize
            DIM AS HDC hDC
            hDC = GetDC(CBHWND)
            GetTextExtentPoint32(hDC, szTmp, LEN(szTmp), &strSize)
            lpMIS->itemWidth = strSize.cx
            lpMIS->itemHeight = 30
            ReleaseDC(CBHWND, hDC)
        END IF
        FUNCTION = TRUE

    CASE WM_DRAWITEM
        DIM AS LPDRAWITEMSTRUCT itemStruct
        itemStruct = (LPDRAWITEMSTRUCT)CBLPARAM
        DIM AS HDC hDC
        hDC = itemStruct->hDC
        SetTextColor(hDC, DARKTEXTCOLOR)
        SetBkMode(hDC, TRANSPARENT)

        IF itemStruct->CtlType = ODT_MENU THEN
            DrawText(hDC, (LPSTR)(itemStruct->itemData), -1, &(itemStruct->rcItem), DT_VCENTER | DT_SINGLELINE)
        ELSE
            itemStruct->rcItem.left += 6
            SetBkColor(hDC, DARKBKCOLOR)
            SetTextColor(hDC, DARKTEXTCOLOR)
            DIM AS HBRUSH hBrush
            hBrush = CreateSolidBrush(DARKBKCOLOR)
            FillRect(hDC, &itemStruct->rcItem, hBrush)
            DeleteObject(hBrush)
            DrawText(hDC, (LPSTR)(itemStruct->itemData), -1, &itemStruct->rcItem, DT_VCENTER | DT_SINGLELINE)
        END IF
        FUNCTION = TRUE

    CASE WM_DESTROY
        IF gDarkMode_Forms[formIndex].hBrush <> NULL THEN
            DeleteObject(gDarkMode_Forms[formIndex].hBrush)
            gDarkMode_Forms[formIndex].hBrush = NULL
        END IF
        IF gDarkMode_Forms[formIndex].hEditBrush <> NULL THEN
            DeleteObject(gDarkMode_Forms[formIndex].hEditBrush)
            gDarkMode_Forms[formIndex].hEditBrush = NULL
        END IF
        IF gDarkMode_Forms[formIndex].hListBoxBrush <> NULL THEN
            DeleteObject(gDarkMode_Forms[formIndex].hListBoxBrush)
            gDarkMode_Forms[formIndex].hListBoxBrush = NULL
        END IF
        FUNCTION = CallWindowProc(gDarkMode_Forms[formIndex].Darkmode_wndProc, CBHWND, CBMSG, CBWPARAM, CBLPARAM)

    END SELECT

    FUNCTION = CallWindowProc(gDarkMode_Forms[formIndex].Darkmode_wndProc, CBHWND, CBMSG, CBWPARAM, CBLPARAM)
END FUNCTION


CALLBACK FUNCTION DarkMode_Dialog_WndProc()
    DIM AS INT formIndex, dlgIndex
    DIM AS INT i

    formIndex = -1
    dlgIndex = -1

    ' Find which form owns this dialog
    FOR i = 0 TO gDarkMode_FormCount - 1
        DIM AS INT j
        FOR j = 0 TO gDarkMode_Forms[i].dialogCount - 1
            IF gDarkMode_Forms[i].dialogs[j] = CBHWND THEN
                formIndex = i
                dlgIndex = j
                EXIT FOR
            END IF
        NEXT
        IF formIndex <> -1 THEN
            EXIT FOR
        END IF
    NEXT

    IF formIndex = -1 THEN
        FUNCTION = 0
    END IF

    SELECT CASE CBMSG

    CASE WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, WM_CTLCOLORDLG, WM_CTLCOLORLISTBOX, WM_CTLCOLORSCROLLBAR
        SetBkColor((HDC)CBWPARAM, DARKBKCOLOR)
        SetTextColor((HDC)CBWPARAM, DARKTEXTCOLOR)
        FUNCTION = (LONGLONG)gDarkMode_Forms[formIndex].hBrush

    CASE WM_MEASUREITEM
        DIM AS LPMEASUREITEMSTRUCT lpMIS
        lpMIS = CAST(LPMEASUREITEMSTRUCT, CBLPARAM)
        IF lpMIS->CtlType = ODT_MENU THEN
            lpMIS->itemHeight = DarkMode_ScaleForDPI(25)
            lpMIS->itemWidth  = DarkMode_ScaleForDPI(40)
        END IF
        FUNCTION = TRUE

    CASE WM_DRAWITEM
        DIM AS LPDRAWITEMSTRUCT itemStruct
        itemStruct = (LPDRAWITEMSTRUCT)CBLPARAM
        DIM AS HDC hDC
        hDC = itemStruct->hDC
        SetTextColor(hDC, DARKTEXTCOLOR)
        SetBkMode(hDC, TRANSPARENT)
        SetBkColor(hDC, DARKBKCOLOR)
        DIM AS HBRUSH hBrush
        hBrush = CreateSolidBrush(DARKBKCOLOR)
        FillRect(hDC, &itemStruct->rcItem, hBrush)
        DeleteObject(hBrush)
        DrawText(hDC, (LPSTR)(itemStruct->itemData), -1, &itemStruct->rcItem, DT_VCENTER | DT_SINGLELINE)
        FUNCTION = TRUE

    END SELECT

    FUNCTION = CallWindowProc(gDarkMode_Forms[formIndex].dialogProcs[dlgIndex], CBHWND, CBMSG, CBWPARAM, CBLPARAM)
END FUNCTION


SUB DarkMode_ChangeMenuBG(hwnd AS HWND, hMenu AS HMENU, NewRGB AS COLORREF)
    DIM mnuInfo AS MENUINFO
    mnuInfo.cbSize = SIZEOF(mnuInfo)
    mnuInfo.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS
    mnuInfo.hbrBack = CreateSolidBrush(NewRGB)
    SetMenuInfo(hMenu, &mnuInfo)
END SUB
