#	include "MarmaladeOptionsSystem.h"

#   include "s3eConfig.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( OptionsSystem, Menge::MarmaladeOptionsSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeOptionsSystem::getOptions( Char * _options, size_t _capacity ) const
	{
		char commandLine[S3E_CONFIG_STRING_MAX] = {0};
		if( s3eConfigGetString( "MENGINE", "CommandLine", commandLine ) == S3E_RESULT_ERROR )
		{
			//printf( "s3eConfigGetString %s:%s return error '%s'\n"
			//	, "MENGINE"
			//	, "CommandLine"
			//	, s3eConfigGetErrorString()
			//	);

			if( _capacity < 1 )
			{
				return false;
			}

			_options[0] = '\0';

			return true;
		}

		if( strlen( commandLine ) >= _capacity )
		{
			return false;
		}

		strcpy( _options, commandLine );

		return true;
	}
}