#	pragma once

namespace lua_boost
{
	template<class T>
	struct safe_result
	{
		safe_result( const T &r, bool v )
			: valid( v )
			, result( r )
		{
		}

		bool valid;
		T result;
	};

	template<>
	struct safe_result<void>
	{
		template< class T >
		safe_result( const T &r, bool v )
			: valid( v )
		{
		}

		bool valid;
	};
}