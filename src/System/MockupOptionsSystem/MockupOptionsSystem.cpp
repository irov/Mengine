#	include "MockupOptionsSystem.h"

#	include "WindowsLayer/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( OptionsSystem, Mengine::MockupOptionsSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	bool MockupOptionsSystem::getOptions( Char * _options, size_t _capacity ) const
	{
		if( _capacity == 0 )
		{
			return false;
		}

		strcpy( _options, "" );

		return true;
	}
}