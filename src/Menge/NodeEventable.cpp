#	include "NodeEventable.h"

#	include "Scriptable.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool NodeEventable::registerEventMethod( const std::string & _name, const std::string & _method  )
	{
		Scriptable * scriptable = getScriptable();
		PyObject * module = scriptable->getScript();

		return Eventable::registerEventListener( _name, _method, module );
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeEventable::loader(XmlElement * _xml)
	{
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_NODE("Event")
			{
				std::string type;
				std::string function;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Type", type );
					XML_CASE_ATTRIBUTE( "Function", function );
				}

				registerEventMethod( type, function );
			}
		}
	}
}