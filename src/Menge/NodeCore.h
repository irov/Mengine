#	pragma once

#	include "Node.h"
#	include "Eventable.h"

#	include "ObjectDeclare.h"

#	include <list>

#	include <map>

namespace Menge
{
	class NodeCore
		: public virtual Node
		, public Eventable
	{
	public:
		NodeCore();

		~NodeCore();
		void destroy() override;

	public:
		bool activate() override;
		void deactivate() override;

		bool isActive() override;

	public:
		void setName( const std::string & _name ) override;
		const std::string & getName() const override;

		void setType( const std::string & _type ) override;
		const std::string & getType() const override;
				
	public:
		void update( float _timing ) override;
		void loader( TiXmlElement *xml ) override;

	public:		
		void debugRender() override;

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