#	pragma once

#	include "Node.h"
#	include "Eventable.h"

#	include "ObjectDeclare.h"

#	include <list>

#	include <map>

namespace Menge
{

	class NodeCore
		: public Node
		, public Eventable
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

	public:
		virtual void registerEvent( const std::string &_name, ScriptObject * _func  );
		virtual ScriptObject * event( const std::string &_name );

		virtual	void setScriptable( ScriptObject * _scriptable );
		virtual ScriptObject * getScriptable();
		virtual bool isScriptable() const;

	protected:

		virtual void _update( float _timing );

		virtual bool _activate();
		virtual void _deactivate();

		virtual void _debugRender();

	protected:
		bool m_active;

		std::string m_name;
		std::string m_type;

		ScriptObject * m_scriptable;
	};

}