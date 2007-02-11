#	include "lua_string.h"

#	include <string.h>

namespace lua_boost
{
	//////////////////////////////////////////////////////////////////////////
	lua_string::lua_string( const char * _str )
	{
		if ( _str != 0 )
		{
			size_t sz = strlen(_str);
			m_buff = new char[ sz + 1 ];
			strcpy_s( m_buff, sz + 1, _str );
		}
		else
		{
			m_buff = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	lua_string::lua_string( const lua_string & _str )
	{
		if ( _str.empty() )
		{
			m_buff = 0;
		}
		else
		{
			size_t sz = _str.size();
			m_buff = new char[ sz + 1 ];
			strcpy_s( m_buff, sz + 1, _str.c_str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void lua_string::assign( const char * _str )
	{
		delete [] m_buff;

		if ( _str != 0 )
		{
			size_t sz = strlen(_str);
			m_buff = new char[ sz + 1 ];
			strcpy_s( m_buff, sz + 1, _str );
		}
		else
		{
			m_buff = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	lua_string::~lua_string()
	{
		delete [] m_buff;
	}
	//////////////////////////////////////////////////////////////////////////
	bool lua_string::empty() const
	{
		return size() == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t lua_string::size() const
	{
		return ( m_buff ) ? strlen(m_buff) : 0 ;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * lua_string::c_str() const
	{
		return m_buff;
	}
}
