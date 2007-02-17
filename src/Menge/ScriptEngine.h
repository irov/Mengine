#	pragma once

#	include "Holder.h"
#	include "Utility/Factory.h"

#	include <string>
#	include <map>
#	include <list>

#	include "lua_boost/lua_boost.h"

namespace Menge
{
	class Node;
	class Scriptable;

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

	public:
		void init();

		void export_function();

		bool doFile(const std::string &_file);
		bool doString(const std::string &_string);

	public:
		const lua_boost::lua_functor * genFunctor( const std::string &_name );

		lua_boost::lua_functor_traits callFunction( const std::string & _name );
		lua_boost::lua_functor_traits_safe callFunctionSafe( const std::string & _name );

	public:
		template<class T>
		lua_boost::lua_class<T> regClass( const std::string & _name )
		{
			return m_luaBoost.reg_class<T>( _name.c_str() );
		}

		lua_boost::lua_reg_module regModule();

		template<class T>
		void refObject( const std::string & _name, T * _this )
		{
			return m_luaBoost.ref_object<T>( _name.c_str(), _this );
		}

	public:
		const std::string & getErrorString();
	
		typedef Scriptable * (*FScriptRegistration)();

		typedef	std::list< Scriptable * > TListScripting;
		typedef	std::list< FScriptRegistration > TListRegister;
		
		static TListScripting & getScriptingList();
		static TListRegister & getRegisterList();

		static void registration( FScriptRegistration _func );
	
	private:
		lua_boost::lua_boost m_luaBoost;

		typedef std::map<std::string, const lua_boost::lua_functor * > TMapLuaFunctor;
		TMapLuaFunctor m_mapLuaFunctor;
	};
}

#	define MENGE_SCRIPT_CLASS_REGISTRATOR( NAME, CLASS );