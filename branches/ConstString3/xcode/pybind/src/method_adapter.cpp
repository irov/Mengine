#	include "pybind/method_adapter.hpp"

#	include "pybind/class_type.hpp"

#	include <string.h>

namespace pybind
{
	namespace detail
	{
		void * meta_cast_scope( void * _self, const char * _scope_name, const char * _name, class_type_scope * scope )
		{
			const char * class_name = scope->get_type();

			if( strcmp( class_name , _scope_name ) == 0 )
			{
				return _self;
			}	
			
			return scope->metacast( _name, _self );			
		}
	}
}

