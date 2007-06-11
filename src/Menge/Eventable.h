#	pragma once

#	include <map>
#	include <string>

class TiXmlElement;

namespace Menge
{
	class ScriptObject;

	class Eventable
	{
	public:
		~Eventable();

	public:
		void registerEvent( const std::string &_name, ScriptObject * _func  );
		ScriptObject * event( const std::string &_name );
		
	public:
		void loader(TiXmlElement *xml);

	private:
		typedef std::map<std::string, ScriptObject *> TMapScriptFunction;
		TMapScriptFunction m_mapScriptFunction;
	};
}