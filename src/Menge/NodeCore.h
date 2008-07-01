#	pragma once

#	include "Node.h"
#	include "NodeResource.h"

#	include "Scriptable.h"
#	include "Eventable.h"

#	include "ObjectDeclare.h"

#	include <list>

#	include <map>

namespace Menge
{
	class NodeCore
		: public virtual Node
		//, public Eventable
		, public NodeResource
	//	, public Scriptable
	{
	public:
		NodeCore();

		~NodeCore();
		void destroy() override;

	public:
//		bool registerEvent( const std::string & _name, const std::string & _method );

	public:
		bool activate() override;
		void deactivate() override;
		bool isActivate() const override;

	public:
		void enable() override;
		void disable() override;
		bool isEnable() override;

		void setUpdatable( bool _updatable ) override;

	public:
		void update( float _timing ) override;
		bool isUpdatable() override;
		bool updatable() override;

	public:
		void loader( XmlElement *xml ) override;

	public:
		Scriptable * getScriptable() override;

	protected:
		virtual void _destroy();
		virtual void _update( float _timing );

		virtual bool _activate();
		virtual void _deactivate();

	protected:
		bool m_active;
		bool m_enable;
		bool m_updatable;

		std::string m_name;
		std::string m_type;
	};

}