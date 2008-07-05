#	pragma once

#	include <string>
#	include "Identity.h"
#	include "Scriptable.h"
#	include "Eventable.h"
#	include "Resource.h"
#	include "InputHandler.h"
#	include "ObjectDeclare.h"

class XmlElement;

namespace Menge
{	
	class NodeForeach;

	class Node
		: public Identity
		, public Resource
		, public Scriptable
		, public Eventable
		, public InputHandler
	{
	public:
		Node();
		virtual ~Node(){};

	public:
		void destroy();

	protected:
		virtual void _destroy();
	
	public:
		bool activate();
		void deactivate();
		bool isActivate() const;

	protected:
		virtual bool _activate();
		virtual void _deactivate();

	public:
		void hide( bool _value );
		bool isHide() const;

	public:
		// kill virtual
		virtual bool isRenderable();

	public:
		bool isUpdatable() const;

	public:
		bool compile();
		void release();
		bool recompile();

	public:
		void enable();
		void disable();
		bool isEnable();

		void setUpdatable( bool _updatable );
		bool updatable();

	public:

		virtual void setParent( Node * _node ) = 0;
		virtual Node* getParent() = 0;

		virtual bool addChildren( Node * _node ) = 0;
		virtual void removeChildren( Node * _node ) = 0;

		virtual Node * getChildren( const std::string & _name, bool _recursion ) = 0;
		virtual bool isChildren( Node * _node ) = 0;

		virtual void foreachChildren( NodeForeach & _foreach ) = 0;

	public:

		virtual void render( bool _enableDebug ) = 0;
	// why virtual? 
		virtual void update( float _timing );
		virtual void loader( XmlElement * xml );

	protected:
		PyObject * _embedded() override;

	public:
		void setListener( PyObject * _listener );

	protected:
		virtual void _setListener();

	protected:
		PyObject * m_listener;

	protected:
		bool m_hide;
		bool m_active;
		bool m_enable;
		bool m_updatable;

		virtual void _update( float _timing );
	};
}
