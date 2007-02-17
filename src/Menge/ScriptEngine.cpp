#	include "ScriptEngine.h"

#	include "Scriptable.h"

#	include "FileEngine.h"

using namespace Menge;

//////////////////////////////////////////////////////////////////////////
ScriptEngine::ScriptEngine()
{
	Holder<ScriptEngine>::keep(this);
}
//////////////////////////////////////////////////////////////////////////
ScriptEngine::~ScriptEngine()
{
	TListScripting & scr_list = ScriptEngine::getScriptingList();

	//for( TListScripting::iterator 
	//	it = scr_list.begin(),
	//	it_end = scr_list.end();
	//it != it_end;
	//++it)
	//{
	//	delete (*it);
	//}

	m_luaBoost.close();
}
//////////////////////////////////////////////////////////////////////////
void ScriptEngine::init()
{
	m_luaBoost.init();
	export_function();

	TListRegister & reg_list = ScriptEngine::getRegisterList();
	TListScripting & scr_list = ScriptEngine::getScriptingList();

	for( TListRegister::iterator 
		it = reg_list.begin(),
		it_end = reg_list.end();
	it != it_end;
	++it)
	{
		Scriptable * impl = (*it)();
		scr_list.push_back(impl);
	}
}
//////////////////////////////////////////////////////////////////////////
bool ScriptEngine::doString(const std::string &_string)
{
	return m_luaBoost.do_buffer( _string.c_str(), _string.size() ) == 0;
}
//////////////////////////////////////////////////////////////////////////
bool ScriptEngine::doFile(const std::string &_file)
{
	FileDataInterface *_data = Holder<FileEngine>::hostage()->openFile(_file);

	if( _data == 0 )
	{
		return false;
	}

	return m_luaBoost.do_buffer( _data->getBuffer(), _data->size() ) == 0;
}
//////////////////////////////////////////////////////////////////////////
const lua_boost::lua_functor * ScriptEngine::genFunctor( const std::string &_name )
{
	TMapLuaFunctor::iterator it_find = m_mapLuaFunctor.find( _name );
	if( it_find != m_mapLuaFunctor.end() )
	{
		return it_find->second;
	}

	const lua_boost::lua_functor * func = 
		new	lua_boost::lua_functor(
		m_luaBoost.functor( _name.c_str() )
		);

	m_mapLuaFunctor.insert( std::make_pair(_name, func ) );

	return func;
}
//////////////////////////////////////////////////////////////////////////
lua_boost::lua_functor_traits ScriptEngine::callFunction( const std::string & _name )
{
	return m_luaBoost.call_function( _name.c_str() );
}
//////////////////////////////////////////////////////////////////////////
lua_boost::lua_functor_traits_safe ScriptEngine::callFunctionSafe( const std::string & _name )
{
	return m_luaBoost.call_function_safe( _name.c_str() );
}
//////////////////////////////////////////////////////////////////////////
lua_boost::lua_reg_module ScriptEngine::regModule()
{
	return lua_boost::lua_reg_module();
}
//////////////////////////////////////////////////////////////////////////
ScriptEngine::TListScripting & ScriptEngine::getScriptingList()
{
	static TListScripting list;
	return list;
}
//////////////////////////////////////////////////////////////////////////
ScriptEngine::TListRegister & ScriptEngine::getRegisterList()
{
	static TListRegister list;
	return list;
}
//////////////////////////////////////////////////////////////////////////
void ScriptEngine::registration( ScriptEngine::FScriptRegistration _func )
{
	TListRegister & list = ScriptEngine::getRegisterList();

	list.push_back( _func );
}