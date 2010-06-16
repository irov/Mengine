#	include "FactoryIdentity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	std::size_t FactoryIdentity::cacheIdentity( const String & _name )
	{
		TMapConstIdentity::const_iterator it_found = m_cacheConstIdentity.find( _name );

		if( it_found == m_cacheConstIdentity.end() )
		{
			std::size_t identity = m_poolConstIdentity.size();
			it_found = m_cacheConstIdentity.insert( std::make_pair(_name, identity) ).first;

			m_poolConstIdentity.push_back( it_found );
		}

		return it_found->second;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t FactoryIdentity::findIdentity( const String & _name ) const
	{
		TMapConstIdentity::const_iterator it_found = m_cacheConstIdentity.find( _name );

		if( it_found == m_cacheConstIdentity.end() )
		{
			return -1;
		}

		return it_found->second;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & FactoryIdentity::getIdentity( std::size_t _identity ) const
	{
		return m_poolConstIdentity[ _identity ]->first;
	}
}
