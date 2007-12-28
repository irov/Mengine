#	pragma once

#	include "Node.h"
#	include "NodeEventable.h"
#	include "NodeResource.h"
#	include "Scriptable.h"

#	include "ObjectDeclare.h"

#	include <list>

#	include <map>

namespace Menge
{
	class NodeCore
		: public virtual Node
		, public NodeEventable
		, public NodeResource
		, public Scriptable
	{
	public:
		NodeCore();

		~NodeCore();
		void destroy() override;

	public:
		bool activate() override;
		void deactivate() override;
		bool isActivate() override;

	public:
		void enable() override;
		void disable() override;
		bool isEnable() override;

		void setUpdatable( bool _updatable ) override;

	public:
		void setName( const std::string & _name ) override;
		const std::string & getName() const override;

		void setType( const std::string & _type ) override;
		const std::string & getType() const override;
				
	public:
		void update( float _timing ) override;
		bool isUpdatable() override;

	public:
		void loader( TiXmlElement *xml ) override;

	public:
		Scriptable * getScriptable() override;

	public:		
		void debugRender() override;

	protected:
		virtual void _destroy();
		virtual void _update( float _timing );

		virtual bool _activate();
		virtual void _deactivate();

		virtual void _debugRender();

	protected:

		void	save() override;

	protected:
		bool m_active;
		bool m_enable;
		bool m_updatable;

		std::string m_name;
		std::string m_type;
	};

}