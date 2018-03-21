#include "Tags.h"

#include <algorithm>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	void Tags::addTag( const ConstString & _tag )
	{
		m_tags.push_back( _tag );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Tags::hasTag( const ConstString & _tag ) const
	{
		TVectorConstString::const_iterator it_found = std::find( m_tags.begin(), m_tags.end(), _tag );

		return it_found != m_tags.end();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Tags::inTags( const Tags & _tag ) const
	{
		for( TVectorConstString::const_iterator
			it = _tag.m_tags.begin(),
			it_end = _tag.m_tags.end();
		it != it_end;
		++it )
		{
			const ConstString & tag = *it;

			if( this->hasTag( tag ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Tags::clear()
	{
		m_tags.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorConstString & Tags::getTags() const
	{
		return m_tags;
	}
	//////////////////////////////////////////////////////////////////////////
	stdex::string Tags::to_str() const
	{
		stdex::string str;

		str += '[';

		for( TVectorConstString::const_iterator
			it = m_tags.begin(),
			it_end = m_tags.end();
		it != it_end;
		++it )
		{
			const ConstString & tag = *it;

			str.append( tag.c_str(), tag.size() );

			str += ' ';
		}

		str += ']';

		return str;
	}
	//////////////////////////////////////////////////////////////////////////
	void Tags::swap( Tags & _tags )
	{
		m_tags.swap( _tags.m_tags );
	}
}