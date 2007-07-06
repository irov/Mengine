#	pragma once

#	include "Node.h"

#	include <map>
#	include <string>

class TiXmlElement;

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class Eventable
		: public virtual Node
	{
	public:
		void removeAllEvent();

	public:
		void registerEvent( const std::string &_name, PyObject * _func  ) override;
		PyObject * getEvent( const std::string &_name ) override;
		
	public:
		void loader(TiXmlElement *xml);

	private:
		typedef std::map<std::string, PyObject *> TMapScriptFunction;
		TMapScriptFunction m_mapScriptFunction;
	};
}