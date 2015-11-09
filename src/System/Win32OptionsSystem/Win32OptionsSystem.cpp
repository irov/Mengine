#	include "Win32OptionsSystem.h"

#	include "WindowsLayer/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( OptionsSystem, Menge::Win32OptionsSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool Win32OptionsSystem::getOptions( Char * _options, size_t _capacity ) const
	{
		const Char * commandLine = GetCommandLineA();

		if( strlen( commandLine ) >= _capacity )
		{
			return false;
		}

		strcpy( _options, commandLine );

		return true;
	}
}