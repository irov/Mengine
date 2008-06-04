#	pragma once

#	include "ObjectDeclare.h"

#	include "Identity.h"
#	include "Resource.h"
#	include "Allocator2D.h"
#	include "BoundingBox.h"
#	include "Scriptable.h"
#	include "Eventable.h"
#	include "InputHandler.h"

#	include "Viewport.h"

#	include "Math/box2.h"

#	include <string>
#	include <list>
#	include <map>


class XmlElement;

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{	
	class Visitor;

	class PyNode;
	class PyListener;

	class Layer2D;


	typedef std::list<class Node *> TContainerChildrens;

	//! Node - базовый pure virtual class
	/*! 
	* 
	*/
	class Node
		: public Identity
		, public Resource
		, public Allocator2D
		, public BoundingBox
		, public Scriptable
		, public Eventable		
		, public InputHandler
	{
	public:
		Node();

		//! Node virtual destructor.
		/*!
		*/
		virtual ~Node();

		//! Node pure virtual. need call this, if you want delete this object
		/*!
		*/
		void destroy();
		virtual void _destroy();

		virtual void visit( Visitor * _visitor );
		virtual void visitChildren( Visitor * _visitor );

	public:
		//! Node pure virtual. activate node
		/*!
		*/
		virtual bool activate();
		virtual bool _activate();

		//! Node pure virtual. deactivate node
		/*!
		*/
		virtual void deactivate();
		virtual void _deactivate();

		//! Node pure virtual. check activated node
		/*!
		\return result of activated
		*/
		bool isActivate() const;

	public:
		//! Node pure virtual. compile node, setup resource for work
		/*!
		\return result of compilated
		*/
		bool compile() override;

		//! Node pure virtual. release node, remove resource for work
		/*!
		*/
		void release() override;

		bool recompile();

	public:
		//! Node pure virtual. enable node
		/*!
		*/
		void enable();

		//! Node pure virtual. disable node
		/*!
		*/
		void disable();

		//! Node pure virtual. check enabled node
		/*!
		\return result of enabled
		*/
		bool isEnable() const;

		void setUpdatable( bool _updatable );
		bool updatable();

	public:

		//! Node pure virtual. set parent node
		/*!
		\param _node the first argument, parent node.
		*/
		void setParent( Node * _node );

		Node* getParent();

		//! Node pure virtual. add children
		/*!
		\param _node the first argument, children node.
		*/
		bool addChildren( Node * _node );

		//! Node pure virtual. remove children
		/*!
		\param _node the first argument, children node.
		*/
		void removeChildren( Node * _node );

		virtual Node * getChildren( const std::string & _name, bool _recursion );

		//! Node pure virtual. check is children
		/*!
		\param _node the first argument, pointer of children
		\return is children?
		*/
		bool isChildren( Node * _node ) const;

		//! Node pure virtual. apply functor for all children in node
		/*!
		\param _foreach the first argument, functor NodeForeach
		*/

		//////////////////////////////////////////////////////////////////////////
		virtual void _addChildren( Node * _node );
		virtual void _removeChildren( Node * _node );

	public:
		//! Node pure virtual. render node
		/*!
		*/
		virtual void render( const Viewport & _viewport );
		void renderSelf( const Viewport & _viewport );
		bool isRenderable() const;

	protected:
		virtual void _render( const Viewport & _viewport );

	public:
		void hide( bool _value );
		bool isHide() const;

	public:
		virtual bool checkVisibility( const Viewport & _viewport );
		void changeVisibility();

	protected:
		virtual bool _checkVisibility( const Viewport & _viewport );

	public:
		//! Node pure virtual. update node
		/*!
		\param _timing the first argument, timing for need update
		*/
		virtual void update( float _timing );
		bool isUpdatable() const;

	protected:
		virtual void _update( float _timing );

	public:
		//! Node pure virtual. setup node from xml
		/*!
		\param xml the first argument, xml element [XmlElement]
		*/
		virtual void loader( XmlElement * xml );

	protected:
		PyObject * _embedded() override;

	public:
		void setListener( PyObject * _listener );

	protected:
		virtual void _setListener();

	public:
		const mt::mat3f & getWorldMatrix();
		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();


		mt::vec2f getScreenPosition( const Viewport & _viewport );

	public:
		void changePivot();

		void _changePivot() override;

	public:
		void setLayer( Layer2D * _layer );
		Layer2D * getLayer();

	public:
		const mt::box2f & getWorldBoundingBox();

		void _changeBoundingBox() override;

	public:
		//! Node pure virtual. debug render this node
		/*!
		*/
		virtual void _debugRender( const Viewport & _viewport );

	protected:
		bool m_active;
		bool m_enable;
		bool m_updatable;

		bool m_hide;
		bool m_visibility;
		bool m_changeVisibility;

		PyObject * m_listener;

		Layer2D * m_layer;

		typedef std::map<std::string, PyObject *> TMapMethods;
		TMapMethods m_methods;
		
		TContainerChildrens m_childrens;

		Node * m_parent;
	};
}
