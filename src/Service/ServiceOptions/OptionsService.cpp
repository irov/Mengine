#	include "OptionsService.h"

#	include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( OptionsService, Menge::OptionsService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGINE_OPTIONS_COMMAND_LINE_MAX
#	define MENGINE_OPTIONS_COMMAND_LINE_MAX 1024
#	endif

	//////////////////////////////////////////////////////////////////////////
	bool OptionsService::_initialize()
	{
		Char commandLine[MENGINE_OPTIONS_COMMAND_LINE_MAX] = {0};

		if( OPTIONS_SYSTEM( m_serviceProvider )
			->getOptions( commandLine, MENGINE_OPTIONS_COMMAND_LINE_MAX ) == false )
		{
			return false;
		}
		
		//const Char * option_next = commandLine;

		const Char * option_begin = strstr( commandLine, "-" );

		while( option_begin != nullptr )
		{
			const Char * option_end = strstr( option_begin + 1, " " );
			
			Option op;

			if( option_end != nullptr )
			{
				size_t len = option_end - option_begin - 1;

				if( len >= MENGINE_OPTIONS_KEY_SIZE )
				{
					return false;
				}
								
				strncpy( op.key, option_begin + 1, len );
				op.key[len] = 0;

				option_begin = strstr( option_end, "-" );
			}
			else
			{
				size_t len = strlen( option_begin + 1 );

				if( len >= MENGINE_OPTIONS_KEY_SIZE )
				{
					return false;
				}

				strcpy( op.key, option_begin + 1 );

				option_begin = nullptr;
			}

			m_options.push_back( op );			
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OptionsService::_finalize()
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	bool OptionsService::hasOption( const Char * _key ) const
	{
		for( TVectorOptions::const_iterator
			it = m_options.begin(),
			it_end = m_options.end();
		it != it_end;
		++it )
		{
			const Option & op = *it;

			if( strcmp( op.key, _key ) != 0 )
			{
				continue;
			}

			return true;
		}

		return false;
	}
}