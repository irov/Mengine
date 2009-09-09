#	pragma once

#	include "Node.h"
#	include "Eventable.h"

namespace Menge
{
	class NodeEventable
		: public virtual Node
		, public Eventable
	{
	public:
		bool registerEvent( const std::string & _name, const std::string & _method );

	public:
		void loader( XmlElement * _xml );
	};
}