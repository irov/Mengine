#	pragma once

#	include <string>
#	include "Identity.h"
#	include "Scriptable.h"
#	include "Eventable.h"
#	include "Resource.h"
#	include "InputHandler.h"

class XmlElement;

namespace Menge
{	
	class NodeForeach;
//	class Scriptable;	

	//! Node - ·‡ÁÓ‚˚È pure virtual class
	/*! 
	* 
	*/
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
		virtual void destroy() = 0;
	
	public:
		void hide( bool _value );
		bool isHide() const;

	public:
		// ”¡–¿“‹ ¬»–“”¿À
		virtual bool isRenderable();

	protected:
		bool m_hide;
		bool m_active;
		bool m_enable;
		bool m_updatable;

	public:
		bool isActivate() const;
		bool isUpdatable() const;

	public:
		virtual bool activate();
		virtual bool _activate();
		virtual void deactivate();
		virtual void _deactivate();

	public:
		bool compile();
		void release();
		bool recompile();

	public:
		virtual void enable();
		virtual void disable();
		virtual bool isEnable();

		virtual void setUpdatable( bool _updatable ) = 0;
		virtual bool updatable() = 0;

	public:

		//! Node pure virtual. set parent node
		/*!
		\param _node the first argument, parent node.
		*/
		virtual void setParent( Node * _node ) = 0;

		virtual Node* getParent() = 0;

		//! Node pure virtual. add children
		/*!
		\param _node the first argument, children node.
		*/
		virtual bool addChildren( Node * _node ) = 0;

		//! Node pure virtual. remove children
		/*!
		\param _node the first argument, children node.
		*/
		virtual void removeChildren( Node * _node ) = 0;

		virtual Node * getChildren( const std::string & _name, bool _recursion ) = 0;

		//! Node pure virtual. check is children
		/*!
		\param _node the first argument, pointer of children
		\return is children?
		*/
		virtual bool isChildren( Node * _node ) = 0;

		//! Node pure virtual. apply functor for all children in node
		/*!
		\param _foreach the first argument, functor NodeForeach
		*/
		virtual void foreachChildren( NodeForeach & _foreach ) = 0;

	public:
		virtual Scriptable * getScriptable() = 0; 

	public:

		//! Node pure virtual. render node
		/*!
		*/
		virtual void render( bool _enableDebug ) = 0;

		//! Node pure virtual. update node
		/*!
		\param _timing the first argument, timing for need update
		*/
		virtual void update( float _timing ) = 0;

		//! Node pure virtual. setup node from xml
		/*!
		\param xml the first argument, xml element [XmlElement]
		*/
		virtual void loader( XmlElement * xml ) = 0;

	protected:
		PyObject * _embedded() override;

	public:
		void setListener( PyObject * _listener );

	protected:
		virtual void _setListener();

	protected:
		PyObject * m_listener;
	};
}
