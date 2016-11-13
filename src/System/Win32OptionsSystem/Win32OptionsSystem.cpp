#	include "Win32OptionsSystem.h"

#	include "WindowsLayer/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( OptionsSystem, Menge::Win32OptionsSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool Win32OptionsSystem::getOptions( TVectorString & _options ) const
	{
		LPCWSTR lpCmdLine = GetCommandLineW();

		if( lpCmdLine == NULL )
		{
			return false;
		}

		int pNumArgs;
		LPWSTR * szArglist = CommandLineToArgvW( lpCmdLine, &pNumArgs );

		if( szArglist == NULL )
		{
			return false;
		}

#   if (WINVER >= 0x0600)
		DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#   else
		DWORD dwConversionFlags = 0;
#   endif

		for( int i = 1; i != pNumArgs; ++i )
		{
			PWSTR arg = szArglist[i];

			CHAR utf_arg[1024];

			int utf8_size = ::WideCharToMultiByte(
				CP_UTF8
				, dwConversionFlags
				, arg
				, -1
				, utf_arg
				, 1024
				, NULL
				, NULL
				);

			_options.push_back( utf_arg );
		}

		LocalFree( szArglist );

		return true;
	}
}