#	pragma once

#	include <algorithm>

namespace Menge
{
	template<class T, class I, class F>
	inline void ObjectForeach( I _begin, I _end, F _f, bool _deep = false )
	{
		struct helper_cast
			: public std::unary_function<T *, void>
		{
			inline helper_cast( F _f, bool _deep )
				: f( _f )
				, deep( _deep )
			{
			}

			inline void operator ()( Node * object )
			{
				if( T* t = dynamic_cast<T*>(object) )
				{
					f(t);
				}
				
				if( deep )
				{
					for( Node *it = object->beginChildren(); 
						it != 0;
						it = object->nextChildren()
						)
					{
						operator () ( it );
					}
				}
			}

			F f;
			bool deep;
		};

		std::for_each( _begin, _end, helper_cast( _f, _deep ) );
	}
}