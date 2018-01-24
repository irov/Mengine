#	include "ToolUtils/ToolUtils.h"

#	include <vector>
#	include <string>
#	include <sstream>

#	include "Logger/Logger.h"
#	include "ToolUtils/ToolUtils.h"

//////////////////////////////////////////////////////////////////////////
static bool MyRegOpenKey( HKEY _category, const WCHAR * _regPath, HKEY * _key )
{
    HKEY hKey;
    LSTATUS status_RegOpenKeyEx = ::RegOpenKeyEx( _category, _regPath, 0, KEY_READ, &hKey );

    if( status_RegOpenKeyEx == ERROR_FILE_NOT_FOUND )
    {
        status_RegOpenKeyEx = RegOpenKeyEx( _category, _regPath, 0, KEY_READ | KEY_WOW64_64KEY, &hKey );  // Check Python x64
    }

    if( status_RegOpenKeyEx != ERROR_SUCCESS )
    {
        return false;
    }

    *_key = hKey;

    return true;
}
//////////////////////////////////////////////////////////////////////////
static bool MyRegOpenKeys( HKEY * _hKey )
{
    HKEY hKey;
    if( MyRegOpenKey( HKEY_CURRENT_USER, L"Software\\Python\\PythonCore\\3.6\\InstallPath", &hKey ) == true )
    {
        *_hKey = hKey;

        return true;
    }

    if( MyRegOpenKey( HKEY_CURRENT_USER, L"Software\\Python\\PythonCore\\3.6-32\\InstallPath", &hKey ) == true )
    {
        *_hKey = hKey;

        return true;
    }

    if( MyRegOpenKey( HKEY_CURRENT_USER, L"Software\\Python\\PythonCore\\3.6-64\\InstallPath", &hKey ) == true )
    {
        *_hKey = hKey;

        return true;
    }

    if( MyRegOpenKey( HKEY_USERS, L"Software\\Python\\PythonCore\\3.6\\InstallPath", &hKey ) == true )
    {
        *_hKey = hKey;

        return true;
    }

    if( MyRegOpenKey( HKEY_USERS, L"Software\\Python\\PythonCore\\3.6-32\\InstallPath", &hKey ) == true )
    {
        *_hKey = hKey;

        return true;
    }

    if( MyRegOpenKey( HKEY_USERS, L"Software\\Python\\PythonCore\\3.6-64\\InstallPath", &hKey ) == true )
    {
        *_hKey = hKey;

        return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
    (void)lpCmdLine;
	(void)nShowCmd;
        	
    HKEY hKey;
    if( MyRegOpenKeys( &hKey ) == false )
    {
        message_error( "invalid RegOpenKeyEx\n"
        );

        return 0;
    }
    
    WCHAR szBuffer[MAX_PATH];
    DWORD dwBufferSize = sizeof( szBuffer );
    if( RegQueryValueEx( hKey, L"WindowedExecutablePath", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize ) != ERROR_SUCCESS )
    {
        message_error( "invalid RegQueryValueEx\n"
        );

        return 0;
    }
	
    printf( "%ls", szBuffer );
    
	return 0;
}