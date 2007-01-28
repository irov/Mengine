#	pragma once

#	include "Keeper.h"

#	include <string>
#	include <map>

#	include "lua_boost/lua_boost.h"

namespace Menge
{
	class ScriptFunction;

	class Node;

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

	public:
		void init();

		void export_function();

		int doFile(const std::string &_file);
		int doString(const std::string &_string);

		const lua_boost::lua_functor * genFunctor( const std::string &_name );

	public:
		const std::string & getErrorString();

	private:
		lua_boost::lua_boost m_luaBoost;

		typedef std::map<std::string, const lua_boost::lua_functor * > TMapLuaFunctor;
		TMapLuaFunctor m_mapLuaFunctor;
	};
}