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
		bool registerEventMethod( const std::string & _name, const std::string & _method );

	public:
		void loader( TiXmlElement * _xml );
	};
}