#	pragma once

#	include "Node.h"

#	include <map>
#	include <string>

class TiXmlElement;

namespace Menge
{
	class ScriptObject;

	class Eventable
		: public virtual Node
	{
	public:
		~Eventable();

	public:
		void registerEvent( const std::string &_name, ScriptObject * _func  ) override;
		ScriptObject * getEvent( const std::string &_name ) override;
		
	public:
		void loader(TiXmlElement *xml);

	private:
		typedef std::map<std::string, ScriptObject *> TMapScriptFunction;
		TMapScriptFunction m_mapScriptFunction;
	};
}