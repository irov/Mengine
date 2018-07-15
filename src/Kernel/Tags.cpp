#include "Tags.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Tags::Tags()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Tags::~Tags()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void Tags::addTag( const ConstString & _tag )
	{
		m_tags.emplace_back( _tag );
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
		for( const ConstString & tag : _tag.m_tags )
		{
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
	String Tags::to_str() const
	{
		String str;

		str += '[';

		for( const ConstString & tag : m_tags )
		{
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