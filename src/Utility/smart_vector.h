#	pragma once

#	include <vector>

namespace Utility
{
	template<class T> 
	class smart_vector
		: public std::vector<T>
	{
	public: 
		smart_vector()
		{
		}

		smart_vector( size_type Size)
			: std::vector<T>( Size )
		{
		}

	public:
		inline T & push_back_reserve()
		{
			size_type s = size();
			resize(s + 1);
			return back();
		}

		inline void erase( iterator it)
		{
			size_type s = size();
			if( s != 0 )
			{
				*it = back();
			}
			pop_back();
		}

		inline void erase( const_iterator it)
		{
			size_type s = size();
			if( s != 0 )
			{
				*it = back();
			}
			pop_back();
		}

	};
}