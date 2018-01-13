#	include "ToolUtils/ToolUtils.h"

#	include <vector>
#	include <string>
#	include <sstream>

#	include "Logger/Logger.h"
#	include "ToolUtils/ToolUtils.h"

//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
    (void)lpCmdLine;
	(void)nShowCmd;
        	
	const WCHAR * regPath = L"Software\\Python\\PythonCore\\3.6-32\\InstallPath";

    HKEY hKey;
    LSTATUS status_RegOpenKeyEx = ::RegOpenKeyEx( HKEY_CURRENT_USER, regPath, 0, KEY_READ, &hKey );
    
    if( status_RegOpenKeyEx == ERROR_FILE_NOT_FOUND )
    {
        status_RegOpenKeyEx = RegOpenKeyEx( HKEY_CURRENT_USER, regPath, 0, KEY_READ | KEY_WOW64_64KEY, &hKey );  // Check Python x64
    }

    if( status_RegOpenKeyEx != ERROR_SUCCESS )
    {
        message_error( "invalid RegOpenKeyEx %ls %d\n"
            , regPath
            , status_RegOpenKeyEx
        );

        return 0;
    }
    
    WCHAR szBuffer[MAX_PATH];
    DWORD dwBufferSize = sizeof( szBuffer );
    if( RegQueryValueEx( hKey, L"WindowedExecutablePath", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize ) != ERROR_SUCCESS )
    {
        message_error( "invalid RegQueryValueEx %ls\n"
            , regPath
        );

        return 0;
    }
	
    printf( "%ls", szBuffer );
    
	return 0;
}