/*(c)IROV*/

#	include "luascript.h"

#	include "luabind/lua_include.hpp"

extern "C"
{
	#	include "lua-5.0.2/include/lauxlib.h"
	#	include "lua-5.0.2/include/lualib.h"
};

CLuaScript::CLuaScript()
: m_LuaState(0)
{
}

CLuaScript::~CLuaScript()
{
//	lua_close(m_LuaState);
}

void CLuaScript::Init()
{
	m_LuaState = lua_open();

	luaopen_base(m_LuaState);
	luaopen_string(m_LuaState);
	luaopen_debug(m_LuaState);
	luaopen_io(m_LuaState);		
	luaopen_math(m_LuaState);
	
	luabind::open(m_LuaState);
		
	m_LuaGlobalVar = luabind::globals(m_LuaState);
}

void CLuaScript::Close()
{
	if( m_LuaState )
	{			
		//DoString("collectgarbage();");
		//lua_close(m_LuaState);
	}		
}

int CLuaScript::DoFile(const std::string &File)
{
	int i_line = luaL_loadfile(m_LuaState,File.c_str());
	if( i_line == 0 )
	{
		lua_pcall(m_LuaState, 0, 0, 0);
	}
	else
	{
		m_Error = lua_tostring(m_LuaState, -1);
		m_Error += "\n";
		lua_pop(m_LuaState, 1);
	}
	return i_line;
}


int CLuaScript::DoBuffer(const char *buff, size_t sz)
{
	int i_line = luaL_loadbuffer(m_LuaState, buff, sz, "laga");
	if( i_line == 0 )
	{
		lua_pcall(m_LuaState, 0, 0, 0);
	}
	else
	{
		// new 21.10
		m_Error += lua_tostring(m_LuaState, -1);
		m_Error += "\n";

		lua_pop(m_LuaState, 1);
	}
	return i_line;
}


int CLuaScript::DoBuffer(const std::vector<char> &buff)
{
	return DoBuffer(&buff[0],buff.size());
}


int CLuaScript::DoString(const std::string& str)
{
	return DoBuffer(str.c_str(),str.size());
} 

TLuaObject CLuaScript::GenObject(const std::string &Type)
{
	return CallFunction<TLuaObject>(Type);
}

//TLuaObject CLuaScript::Eval(const std::string &Expression)
//{
//	int i_line = luaL_loadbuffer(
//		m_LuaState, 
//		Expression.c_str(), 
//		Expression.size(), 
//		"eval");
//
//	if ( i_line )
//	{
//		throw luabind::error(m_LuaState); 
//	}
//
//	if (lua_pcall(m_LuaState, 0, 1, 0))
//	{
//		throw luabind::error(m_LuaState);
//	}
//	else
//	{
//		lua_pop(m_LuaState, 1);
//	}
//
//	luabind::object o(m_LuaState);
//	o.set();
//	return o;
//}

bool CLuaScript::IsLuaObject(const std::string &Name)
{
	lua_pushstring( m_LuaState, Name.c_str() );
	lua_gettable( m_LuaState, LUA_GLOBALSINDEX);

	bool object_nil = lua_isnil( m_LuaState, -1 );
	lua_pop( m_LuaState, 1 );

	return object_nil == false;
}

TLuaObject CLuaScript::GetLuaObject(const std::string &Name)const
{
	return m_LuaGlobalVar[Name];
}

void CLuaScript::SetLuaObject(const std::string &Name, const std::string &Value)
{
	m_LuaGlobalVar[Name] = Value;
}