#ifdef _WIN32_WINNT	
#   undef _WIN32_WINNT
#   define _WIN32_WINNT 0x0500
#endif

#ifdef _WIN32_WINDOWS
#   undef _WIN32_WINDOWS
#   define _WIN32_WINDOWS 0x0500
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "shlobj.h"

#include <string>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure( HWND, UINT, WPARAM, LPARAM );

/*  Make the class name into a global variable  */
WCHAR szClassName[] = L"RegistrationExport";

int WINAPI WinMain( HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil )
{
    (void)lpszArgument;
    (void)hPrevInstance;

    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof( WNDCLASSEX );

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wincl.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
    wincl.hCursor = LoadCursor( NULL, IDC_ARROW );
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    /* Register the window class, and if it fails quit the program */
    if( !RegisterClassEx( &wincl ) )
        return EXIT_FAILURE;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
        0,                   /* Extended possibilites for variation */
        szClassName,         /* Classname */
        L"RegistrationExport",       /* Title Text */
        WS_SYSMENU,          /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        544,                 /* The programs width */
        200,                 /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );

    /* Make the window visible on the screen */
    ShowWindow( hwnd, nFunsterStil );

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while( GetMessage( &messages, NULL, 0, 0 ) )
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage( &messages );
        /* Send message to WindowProcedure */
        DispatchMessage( &messages );
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return EXIT_SUCCESS;
}

#define ID_BTN_Game 1
#define ID_BTN_Params 2
#define ID_BTN_Art 3
#define ID_BTN_Ok 4
#define ID_TEXTBOX_Value 10
#define ID_TEXTBOX_Game 11
#define ID_TEXTBOX_Params 12
#define ID_TEXTBOX_Art 13

static HWND hwndText_Game;
static HWND hwndText_Params;
static HWND hwndText_Art;
static HWND hwndText_Value;
static HWND hwndText_PathGame;
static HWND hwndText_PathParams;
static HWND hwndText_PathArt;
static HWND hwndText_Ok;

void SetElementPathText( HWND element, HWND hwnd )
{
    BROWSEINFO bi;
    memset( &bi, 0, sizeof( bi ) );

    bi.ulFlags = BIF_USENEWUI;
    bi.hwndOwner = hwnd;
    bi.lpszTitle = NULL;

    LPITEMIDLIST pIDL = ::SHBrowseForFolder( &bi );
    if( pIDL != NULL )
    {
        // Create a buffer to store the path, then 
        // get the path.
        WCHAR buffer[_MAX_PATH] = {'\0'};
        if( ::SHGetPathFromIDList( pIDL, buffer ) != 0 )
        {
            SetWindowText( element, buffer );
        }
    }
}

LRESULT CALLBACK WindowProcedure( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch( message )                  /* handle the messages */
    {
    case WM_CREATE:
        {
            hwndText_Value = CreateWindow( TEXT( "EDIT" ), TEXT( "ProjectName" ),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                10, 10, 400, 20,
                hwnd, (HMENU)ID_TEXTBOX_Value, NULL, NULL );

            hwndText_PathGame = CreateWindow( TEXT( "button" ), TEXT( "Path Game" ),
                WS_VISIBLE | WS_CHILD,
                10, 40, 100, 20,
                hwnd, (HMENU)ID_BTN_Game, NULL, NULL );

            hwndText_Game = CreateWindow( TEXT( "EDIT" ), TEXT( "" ),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                120, 40, 400, 20,
                hwnd, (HMENU)ID_TEXTBOX_Game, NULL, NULL );

            hwndText_PathParams = CreateWindow( TEXT( "button" ), TEXT( "Path Params" ),
                WS_VISIBLE | WS_CHILD,
                10, 70, 100, 20,
                hwnd, (HMENU)ID_BTN_Params, NULL, NULL );

            hwndText_Params = CreateWindow( TEXT( "EDIT" ), TEXT( "" ),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                120, 70, 400, 20,
                hwnd, (HMENU)ID_TEXTBOX_Params, NULL, NULL );

            hwndText_PathArt = CreateWindow( TEXT( "button" ), TEXT( "Path Art" ),
                WS_VISIBLE | WS_CHILD,
                10, 100, 100, 20,
                hwnd, (HMENU)ID_BTN_Art, NULL, NULL );

            hwndText_Art = CreateWindow( TEXT( "EDIT" ), TEXT( "" ),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                120, 100, 400, 20,
                hwnd, (HMENU)ID_TEXTBOX_Art, NULL, NULL );

            hwndText_Ok = CreateWindow( TEXT( "button" ), TEXT( "Ok" ),
                WS_VISIBLE | WS_CHILD,
                10, 130, 80, 20,
                hwnd, (HMENU)ID_BTN_Ok, NULL, NULL );


            break;

        }
    case WM_COMMAND:
        {
            //action for button
            if( LOWORD( wParam ) == ID_BTN_Game )
            {
                //create variables
                int length = GetWindowTextLength( hwndText_Game ) + 1;

                WCHAR title[500] = L"";

                GetWindowText( hwndText_Game, title, length );
                SetElementPathText( hwndText_Game, hwnd );

                length = GetWindowTextLength( hwndText_Game ) + 1;

                WCHAR val[500] = L"";
                GetWindowText( hwndText_Game, val, length );

                wcscat( val, L"/" );
                for( int i = 0; i < length; i++ )
                {
                    WCHAR c = val[i];
                    if( c == 92 ) val[i] = 47;
                }

                SetWindowText( hwndText_Game, val );
            }
            else if( LOWORD( wParam ) == ID_BTN_Params )
            {
                //create variables
                int length = GetWindowTextLength( hwndText_Params ) + 1;

                WCHAR title[500] = L"";
                GetWindowText( hwndText_Params, title, length );
                SetElementPathText( hwndText_Params, hwnd );

                length = GetWindowTextLength( hwndText_Params ) + 1;

                WCHAR val[500] = L"";
                GetWindowText( hwndText_Params, val, length );

                wcscat( val, L"/" );
                for( int i = 0; i < length; i++ )
                {
                    WCHAR c = val[i];
                    if( c == 92 ) val[i] = 47;
                }
                SetWindowText( hwndText_Params, val );
            }
            else if( LOWORD( wParam ) == ID_BTN_Art )
            {
                //create variables
                int length = GetWindowTextLength( hwndText_Art ) + 1;

                WCHAR title[500] = L"";
                GetWindowText( hwndText_Art, title, length );
                SetElementPathText( hwndText_Art, hwnd );

                length = GetWindowTextLength( hwndText_Art ) + 1;

                WCHAR val[500] = L"";
                GetWindowText( hwndText_Art, val, length );

                wcscat( val, L"/" );
                for( int i = 0; i < length; i++ )
                {
                    WCHAR c = val[i];
                    if( c == 92 ) val[i] = 47;
                }
                SetWindowText( hwndText_Art, val );
            }
            else if( LOWORD( wParam ) == ID_BTN_Ok )
            {
                int length = GetWindowTextLength( hwndText_Value ) + 1;
                WCHAR val[500] = L"";
                WCHAR val2[500] = L"";

                GetWindowText( hwndText_Value, val, length );

                wcscat( val2, L"Software\\Mengine\\" );
                wcscat( val2, val );


                HKEY key;
                if( RegCreateKeyEx( HKEY_CURRENT_USER, val2, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL ) != ERROR_SUCCESS )
                {
                    RegCloseKey( key );
                    LPCWSTR Caption = L"Error";
                    MessageBox( NULL, L"Cant Write !!!!!!!!!!!!!!!!!!", Caption, NULL );
                }
                else
                {
                    int length_PathGame = GetWindowTextLength( hwndText_Game ) + 1;
                    WCHAR val_PathGame[500] = L"";
                    GetWindowText( hwndText_Game, val_PathGame, length_PathGame );

                    RegSetValueEx( key, L"MENGINE_PROJECT_PATH", 0, REG_SZ, (LPBYTE)val_PathGame, (DWORD)wcslen( val_PathGame ) * sizeof( wchar_t ) );

                    int length_PathParams = GetWindowTextLength( hwndText_Params ) + 1;
                    WCHAR val_PathParams[500] = L"";
                    GetWindowText( hwndText_Params, val_PathParams, length_PathParams );

                    int length_PathArt = GetWindowTextLength( hwndText_Art ) + 1;
                    WCHAR val_PathArt[500] = L"";
                    GetWindowText( hwndText_Art, val_PathArt, length_PathArt );

                    RegSetValueEx( key, L"MENGINE_PROJECT_PATH", 0, REG_SZ, (LPBYTE)val_PathGame, (DWORD)wcslen( val_PathGame ) * sizeof( wchar_t ) );
                    RegSetValueEx( key, L"MENGINE_PARAMS_PATH", 0, REG_SZ, (LPBYTE)val_PathParams, (DWORD)wcslen( val_PathParams ) * sizeof( wchar_t ) );
                    RegSetValueEx( key, L"MENGINE_ART_PATH", 0, REG_SZ, (LPBYTE)val_PathArt, (DWORD)wcslen( val_PathArt ) * sizeof( wchar_t ) );
                    RegCloseKey( key );

                    LPCWSTR Caption = L"Ok";
                    MessageBox( NULL, L"All Ok", Caption, NULL );
                }
            }
            break;
        }
    case WM_DESTROY:
        PostQuitMessage( 0 );       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc( hwnd, message, wParam, lParam );
    }

    return 0;
}
