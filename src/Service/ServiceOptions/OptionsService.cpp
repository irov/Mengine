#	include "OptionsService.h"

#	include <string.h>


//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( OptionsService, Menge::OptionsService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void OptionsService::setArgs( const TVectorString & _args )
	{
		for( TVectorString::const_iterator
			it = _args.begin(),
			it_end = _args.end();
		it != it_end;
		++it )
		{
			const String & arg = *it;

			const Char * option_str = arg.c_str();

			const Char * option_key_str = strchr( option_str, '-' );

			if( option_key_str == nullptr )
			{
				continue;
			}

			option_key_str += 1;

			const Char * option_value_str = strchr( option_key_str, ':' );

			Option op;

			if( option_value_str == nullptr )
			{
				strcpy( op.key, option_key_str );

				op.value[0] = '\0';
			}
			else
			{
				size_t key_size = option_value_str - option_key_str;

				strncpy( op.key, option_key_str, key_size );
				op.key[key_size] = '\0';

				strcpy( op.value, option_value_str + 1 );
			}

			m_options.push_back( op );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool OptionsService::_initialize()
	{
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
	//////////////////////////////////////////////////////////////////////////
	const Char * OptionsService::getOptionValue( const Char * _key ) const
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

			return op.value;
		}

		return nullptr;
	}
}