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
		virtual bool checkVisibility( const Viewport & _viewport );
		void changeVisibility();

	protected:
		virtual bool _checkVisibility( const Viewport & _viewport );

	protected:
		bool m_visibility;
		bool m_changeVisibility;

	public:
		void setLayer( Layer2D * _layer );
		Layer2D * getLayer() const;
	public:
		void changePivot();

		void _changePivot() override;

	protected:
		Layer2D * m_layer;

	public:
		virtual void render( const Viewport & _viewport, bool _enableDebug );
		virtual void _render( const Viewport & _viewport, bool _enableDebug );
		//void renderSelf( const Viewport & _viewport, bool _enableDebug );
		bool isRenderable() const;

	public:
		void destroy();

		virtual void visit( Visitor * _visitor );
		virtual void visitChildren( Visitor * _visitor );

	public:
		const mt::mat3f & getWorldMatrix();
		mt::vec2f getWorldPosition();
		const mt::vec2f & getWorldDirection();


		mt::vec2f getScreenPosition();

	public:
		void setParent( Node * _node );
		Node* getParent();
		bool addChildren( Node * _node );
		void removeChildren( Node * _node );
		virtual Node * getChildren( const std::string & _name, bool _recursion );
		bool isChildren( Node * _node, bool _recursive ) const;
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

		////
		virtual void setAlpha( float _alpha ) 
		{
			for( TContainerChildrens::iterator it = m_childrens.begin(), it_end = m_childrens.end();
				it != it_end;
				it++ )
			{
				(*it)->setAlpha( _alpha );
			}
		}
		virtual void alphaTo( float _alpha, float _time ) 
		{
			for( TContainerChildrens::iterator it = m_childrens.begin(), it_end = m_childrens.end();
				it != it_end;
				it++ )
			{
				(*it)->alphaTo( _alpha, _time );
			}
		}
		virtual void colorToStop()
		{
			for( TContainerChildrens::iterator it = m_childrens.begin(), it_end = m_childrens.end();
				it != it_end;
				it++ )
			{
				(*it)->colorToStop();
			}
		}

	protected:
		virtual bool _activate();
		virtual void _deactivate();

	public:
		virtual void hide( bool _value );
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
