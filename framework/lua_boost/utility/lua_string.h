#	pragma once

namespace lua_boost
{
	class lua_string
	{
	public:
		lua_string( const char * _str );
		lua_string( const lua_string & _str );

		~lua_string();

		void assign( const char * _str );
		
		bool empty() const;
		size_t size() const;

		const char * c_str() const;
		
	private:
		char *m_buff;
	};
}