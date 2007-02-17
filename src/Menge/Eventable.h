#	pragma once

#	include <map>
#	include <string>

namespace lua_boost
{
	class lua_functor;
}

class TiXmlElement;

namespace Menge
{
	class Eventable
	{
	public:
		void registerEvent( const std::string &_name, const lua_boost::lua_functor * _func  );
		const lua_boost::lua_functor * event( const std::string &_name );
		
	public:
		void loader(TiXmlElement *xml);

	private:
		typedef std::map<std::string, const lua_boost::lua_functor *> TMapScriptFunction;
		TMapScriptFunction m_mapScriptFunction;
	};
}