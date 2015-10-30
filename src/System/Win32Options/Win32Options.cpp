#	include "Win32Options.h"

#	include "WindowsLayer/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Options, Menge::Win32Options );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool Win32Options::_initialize()
	{
		const Char * commandLine = GetCommandLineA();
		
		const Char * option_next = commandLine;

		while( option_next != nullptr )
		{
			const Char * option_begin = strstr( option_next, " -" );
			const Char * option_end = strstr( option_begin + 1, " " );

			Option op;

			if( option_end != nullptr )
			{
				size_t len = option_end - option_begin - 2;

				if( len >= MENGINE_OPTIONS_KEY_SIZE )
				{
					return false;
				}
								
				strncpy( op.key, option_begin + 2, len );
				op.key[len] = 0;
			}
			else
			{
				size_t len = strlen( option_begin + 2 );

				if( len >= MENGINE_OPTIONS_KEY_SIZE )
				{
					return false;
				}

				strcpy( op.key, option_begin + 2 );
			}

			m_options.push_back( op );

			option_next = option_end;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Options::_finalize()
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Options::hasOption( const Char * _key ) const
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