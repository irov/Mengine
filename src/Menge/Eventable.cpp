#	include "Eventable.h"

#	include "ScriptEngine.h"

#	include "XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void Eventable::registerEvent( const std::string &_name, const lua_boost::lua_functor * _func  )
	{
		TMapScriptFunction::iterator it_find = m_mapScriptFunction.find(_name);

		if( it_find == m_mapScriptFunction.end() )
		{
			m_mapScriptFunction.insert(std::make_pair(_name,_func));		
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const lua_boost::lua_functor * Eventable::event( const std::string &_name )
	{
		TMapScriptFunction::iterator it_find = m_mapScriptFunction.find(_name);

		if( it_find == m_mapScriptFunction.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::loader(TiXmlElement *xml)
	{
		XML_FOR_EACH_FORCE(xml)
		{
			XML_CHECK_NODE("Event")
			{
				XML_DEF_ATTRIBUTES_NODE(Type);
				XML_DEF_ATTRIBUTES_NODE(Function);

				ScriptEngine *scriptEng = Holder<ScriptEngine>::hostage();

				const lua_boost::lua_functor * scriptFunction = scriptEng->genFunctor(Function);

				registerEvent(Type, scriptFunction);
			}
		}
	}
}