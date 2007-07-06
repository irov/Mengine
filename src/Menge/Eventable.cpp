#	include "Eventable.h"

#	include "ScriptEngine.h"

#	include "XmlParser.h"

#	include "ScriptObject.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void Eventable::removeAllEvent()
	{
		for each( const TMapScriptFunction::value_type & it in m_mapScriptFunction )
		{
			ScriptEngine::decref( it.second );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::registerEvent( const std::string &_name, PyObject * _func  )
	{
		TMapScriptFunction::iterator it_find = m_mapScriptFunction.find(_name);

		if( it_find == m_mapScriptFunction.end() )
		{
			Holder<ScriptEngine>::hostage()
				->incref( _func );

			m_mapScriptFunction.insert(std::make_pair(_name,_func));		
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Eventable::getEvent( const std::string &_name )
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

				PyObject * scriptFunction = scriptEng->genFunctor(Function);

				registerEvent(Type, scriptFunction);
			}
		}
	}
}