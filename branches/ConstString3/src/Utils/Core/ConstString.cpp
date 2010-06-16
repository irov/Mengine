#	include "ConstString.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConstString ConstStringManager::cacheString( const String & _name )
	{
		TMapConstString::const_iterator it_found = m_cacheConstString.find( _name );

		if( it_found == m_cacheConstString.end() )
		{
			std::size_t identity = m_poolConstString.size();
			it_found = m_cacheConstString.insert( std::make_pair(_name, identity) ).first;

			m_poolConstString.push_back( it_found );
		}

		return ConstString(it_found->second, this);
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ConstStringManager::findString( const String & _name ) const
	{
		TMapConstString::const_iterator it_found = m_cacheConstString.find( _name );

		if( it_found == m_cacheConstString.end() )
		{
			return -1;
		}

		return it_found->second;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & ConstStringManager::getString( std::size_t _identity ) const
	{
		return m_poolConstString[ _identity ]->first;
	}
}
