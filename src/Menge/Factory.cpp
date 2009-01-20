#	include "Factory.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void Factory::unregistration( const String & _key )
	{
		m_generator.erase( _key );
	}
	//////////////////////////////////////////////////////////////////////////
	void Factory::registration( const String & _key, TGenFunc _func )
	{		
		TMapGenerator::const_iterator it_find = m_generator.find( _key );

		if( it_find == m_generator.end() )
		{
			m_generator.insert( std::make_pair( _key, _func ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void * Factory::generate( const String & _name, void * _data  )
	{
		TMapGenerator::const_iterator it_find = m_generator.find( _name );

		if( it_find == m_generator.end() )
		{
			return 0;
		}

		return it_find->second( _data );
	}
}
