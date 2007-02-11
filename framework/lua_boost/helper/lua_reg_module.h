#	pragma once

namespace lua_boost
{
	class lua_reg_module
	{		
	public:

		template<class T>
		void operator [] ( const lua_class<T> & _class )
		{			
			_class.end_reg();
		}
	};
}