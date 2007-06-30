#	pragma once

#	include "Node.h"
#	include "Eventable.h"
#	include "Scriptable.h"

#	include "ObjectDeclare.h"

#	include <list>

#	include <map>

namespace Menge
{
	class NodeCore
		: public virtual Node
		, public Eventable
		, public Scriptable
	{
	public:
		NodeCore();

		virtual ~NodeCore();
		virtual void destroy();

	public:
		virtual bool activate();
		virtual void deactivate();

		virtual bool isActive();

	public:
		virtual void setName( const std::string & _name );
		virtual const std::string & getName() const;

		virtual void setType( const std::string & _type );
		virtual const std::string & getType() const;

	public:
		virtual void update( float _timing );
		virtual void loader( TiXmlElement *xml );

	public:
		virtual void debugRender();

	protected:

		virtual void _update( float _timing );

		virtual bool _activate();
		virtual void _deactivate();

		virtual void _debugRender();

	protected:
		bool m_active;

		std::string m_name;
		std::string m_type;

	};

}