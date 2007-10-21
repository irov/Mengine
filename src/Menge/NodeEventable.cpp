#	include "NodeEventable.h"

#	include "Scriptable.h"

#	include "XmlParser/XmlParser.h"

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
	void NodeEventable::loader(TiXmlElement * _xml)
	{
		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_NODE("Event")
			{
				XML_DEF_ATTRIBUTES_NODE(Type);
				XML_DEF_ATTRIBUTES_NODE(Function);

				registerEventMethod( Type, Function );
			}
		}
	}
}