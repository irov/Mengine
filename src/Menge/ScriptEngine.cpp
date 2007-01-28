#	include "ScriptEngine.h"

#	include "FileEngine.h"

using namespace Menge;

ScriptEngine::ScriptEngine()
{
	Keeper<ScriptEngine>::keep(this);
}
ScriptEngine::~ScriptEngine()
{
	m_luaBoost.close();
}
//////////////////////////////////////////////////////////////////////////
void ScriptEngine::init()
{
	m_luaBoost.init();
	export_function();
}
//////////////////////////////////////////////////////////////////////////
int ScriptEngine::doString(const std::string &_string)
{
	return m_luaBoost.do_buffer( _string.c_str(), _string.size() );
}
//////////////////////////////////////////////////////////////////////////
int ScriptEngine::doFile(const std::string &_file)
{
	FileDataInterface *_data = Keeper<FileEngine>::hostage()->openFile(_file);

	return m_luaBoost.do_buffer( _data->getBuffer(), _data->size() );
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