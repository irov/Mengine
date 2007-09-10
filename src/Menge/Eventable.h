#	pragma once

#	include "Node.h"

#	include <map>
#	include <string>

class TiXmlElement;

namespace Menge
{
	class Event;

	class Eventable
		: public virtual Node
	{
	public:
		Eventable();
		~Eventable();

	public:
		void removeAllEvent();

	public:
		void registerEvent( const std::string &_name, Event * _func  ) override;
		Event * getEvent( const std::string &_name ) override;
		
	public:
		void loader(TiXmlElement * _xml);

	private:
		typedef std::map<std::string, Event *> TMapEvent;
		TMapEvent m_mapEvent;
	};
}