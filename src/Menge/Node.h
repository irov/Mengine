#	pragma once

#	include <string>
#	include "Identity.h"
#	include "Scriptable.h"
#	include "Eventable.h"
#	include "Resource.h"
#	include "InputHandler.h"
#	include "BoundingBox.h"
#	include "ObjectDeclare.h"
#	include "Viewport.h"
#	include "Allocator2D.h"
#	include <list>

class XmlElement;

namespace Menge
{	
	class Visitor;
	class Layer2D;
	typedef std::list<class Node *> TContainerChildrens;

	class NodeForeach;

	class Node
		: public Identity
		, public Resource
		, public Scriptable
		, public Eventable
		, public BoundingBox
		, public Allocator2D
		, public InputHandler
	{
	public:
		Node();
		virtual ~Node(){};

	public:
		void setLayer( Layer2D * _layer );
		Layer2D * getLayer() const;
	public:
		void changePivot();

		void _changePivot() override;

	protected:
		Layer2D * m_layer;

	protected:
		virtual void _render( const Viewport & _viewport, bool _enableDebug );

	public:
		virtual void render( const Viewport & _viewport );
		void renderSelf( const Viewport & _viewport );
		bool isRenderable() const;

	public:
		void destroy();

		virtual void visit( Visitor * _visitor );
		virtual void visitChildren( Visitor * _visitor );

	public:
		const mt::mat3f & getWorldMatrix();
		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();


		mt::vec2f getScreenPosition();

	public:
		void setParent( Node * _node );
		Node* getParent();
		bool addChildren( Node * _node );
		void removeChildren( Node * _node );
		virtual Node * getChildren( const std::string & _name, bool _recursion );
		bool isChildren( Node * _node ) const;
		virtual void _addChildren( Node * _node );
		virtual void _removeChildren( Node * _node );

	protected:
		TContainerChildrens m_childrens;

		Node * m_parent;

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
		bool isUpdatable() const;

	public:
		bool compile();
		void release();
		bool recompile();

	public:
		void enable();
		void disable();
		bool isEnable() const;

		void setUpdatable( bool _updatable );
		bool updatable();

	public:

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

	public:
		const mt::box2f & getWorldBoundingBox();

		void _changeBoundingBox() override;
	};
}
