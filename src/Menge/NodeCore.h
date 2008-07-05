#	pragma once

#	include "Node.h"

#	include "Scriptable.h"
#	include "Eventable.h"

#	include "ObjectDeclare.h"

#	include <list>

#	include <map>

namespace Menge
{
	class NodeCore
		: public virtual Node
	{
	public:
		NodeCore();

		~NodeCore();
		void destroy() override;

	public:

	public:
		void setUpdatable( bool _updatable ) override;

	public:
		void update( float _timing ) override;
		bool updatable() override;

	public:
		void loader( XmlElement *xml ) override;

	public:
		Scriptable * getScriptable() override;

	protected:
		virtual void _destroy();
		virtual void _update( float _timing );
	};

}