#	include "BaseScriptFunctionRegistration.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef std::list< class BaseScriptFunctionRegistration * > TListRegistration;
	//////////////////////////////////////////////////////////////////////////
	TListRegistration & listRegistration()
	{
		static TListRegistration s_listRegistration;
		return s_listRegistration;
	}
	//////////////////////////////////////////////////////////////////////////
	BaseScriptFunctionRegistration::BaseScriptFunctionRegistration()
	{		
		listRegistration().push_back(this);		
	}
	//////////////////////////////////////////////////////////////////////////
	void BaseScriptFunctionRegistration::registration( lua_State * _luaState )
	{
		std::for_each( 
			listRegistration().begin(), 
			listRegistration().end(),
			std::bind2nd( std::mem_fun( &_register ), _luaState ) 
			);
	}
}