#	include "Eventable.h"

#	include "ScriptEngine.h"
#	include "Scriptable.h"

#	include "XmlParser/XmlParser.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Eventable::Eventable()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Eventable::~Eventable()
	{
		removeAllEvent();
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::removeAllEvent()
	{
		for each( const TMapEvent::value_type & it in m_mapEvent )
		{
			ScriptEngine::decref( it.second );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( const std::string & _name, const std::string & _method  )
	{
		Scriptable * scriptable = getScriptable();
		PyObject * module = scriptable->getScript();

		return registerEvent( _name, module, _method );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( const std::string & _name, PyObject * _module, const std::string & _method  )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find != m_mapEvent.end() )
		{
			return false;
		}

		PyObject * event = Holder<ScriptEngine>::hostage()
			->getModuleFunction( _module, _method );

		if( event == 0 )
		{
			return false;
		}

		ScriptEngine::incref( event );

		m_mapEvent.insert(std::make_pair( _name, event ));

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( const std::string & _name, PyObject * _callback )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find != m_mapEvent.end() )
		{
			return false;
		}

		if( _callback == 0 )
		{
			return false;
		}

		ScriptEngine::incref( _callback );

		m_mapEvent.insert(std::make_pair( _name, _callback ));

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Eventable::getEvent( const std::string & _name )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find == m_mapEvent.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::callEvent( const std::string & _name, const char * _format, ... )
	{
		TMapEvent::iterator it_find = m_mapEvent.find( _name );

		if( it_find == m_mapEvent.end() )
		{
			return;
		}

		va_list valist;
		va_start(valist, _format);

		Holder<ScriptEngine>::hostage()
			->callFunction( it_find->second, _format, valist );

		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( bool & _result, const std::string & _name, const char * _format, ... )
	{
		TMapEvent::iterator it_find = m_mapEvent.find( _name );

		if( it_find == m_mapEvent.end() )
		{
			return false; 
		}

		va_list valist;
		va_start(valist, _format);

		PyObject * result = 
			Holder<ScriptEngine>::hostage()
			->callFunction( it_find->second, _format, valist );

		va_end( valist );

		_result = pybind::convert::to_bool( result );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::loader(TiXmlElement * _xml)
	{
		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_NODE("Event")
			{
				XML_DEF_ATTRIBUTES_NODE(Type);
				XML_DEF_ATTRIBUTES_NODE(Function);

				registerEvent( Type, Function );
			}
		}
	}
}