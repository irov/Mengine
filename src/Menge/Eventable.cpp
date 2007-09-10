#	include "Eventable.h"

#	include "ScriptEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "Event.h"

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
			delete it.second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::registerEvent( const std::string &_name, Event * _event  )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find == m_mapEvent.end() )
		{
			m_mapEvent.insert(std::make_pair( _name, _event ));		
		}            
	}
	//////////////////////////////////////////////////////////////////////////
	Event * Eventable::getEvent( const std::string & _name )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find == m_mapEvent.end() )
		{
			return 0;
		}

		return it_find->second;
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

				//ScriptEngine *scriptEng = Holder<ScriptEngine>::hostage();

				//Event * event = scriptEng->genEvent( Function );

				//registerEvent( Type, event );
			}
		}
	}
}