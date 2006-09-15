#	pragma once

#	include "Manager/ErrorMessage.h"

#	define TRY_CATCH_ERROR_ENABLE

#	ifdef TRY_CATCH_ERROR_ENABLE 
#	define TRY_SCRIPT_ERROR try{

#	define CATCH_SCRIPT_ERROR_IMPL\
	catch(const luabind::error &er)\
	{\
	lua_State* L = er.state();\
	const char *Error = lua_tostring(L, -1);\
	ErrorMessage(Error);\
	}\
	catch(...){throw;}

#	define CATCH_SCRIPT_ERROR\
	}CATCH_SCRIPT_ERROR_IMPL

#	define TRY_SCRIPT_FUNCTION( Func )\
	try{ Func; }\
	CATCH_SCRIPT_ERROR_IMPL


#	else
#	define TRY_SCRIPT_ERROR 
#	define CATCH_SCRIPT_ERROR 
#	endif