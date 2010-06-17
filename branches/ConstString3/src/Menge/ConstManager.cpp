#	include "ConstManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConstString ConstManager::genString( const String & _value )
	{
		TMapConstString::iterator it_found = m_cacheConstString.find( _value );

		if( it_found == m_cacheConstString.end() )
		{
			ConstStringHolder holder(this);
			TListConstString::iterator it_insert = m_poolConstString.insert( m_poolConstString.end(), holder);
			
			it_found = m_cacheConstString.insert( std::make_pair(_value, it_insert) ).first;
			it_insert->cache( it_found );
		}

		return ConstString(&*it_found->second);
	}
	//////////////////////////////////////////////////////////////////////////
	void ConstManager::removeString( TMapConstString::iterator _iterator )
	{
		m_poolConstString.erase( _iterator->second );
		m_cacheConstString.erase( _iterator );
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Utils
	{
		const ConstString & emptyConstString()
		{
			static ConstString cs;
			return cs;
		}
	}
}
