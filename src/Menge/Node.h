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
#	include "ColourValue.h"
#	include <list>

#	include "ValueInterpolator.h"

class XmlElement;

namespace Menge
{	
	class Visitor;
	class Layer2D;
	

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
		virtual ~Node();

	public:
		virtual bool checkVisibility();
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
		void invalidateWorldMatrix();
		void _invalidateWorldMatrix() override;

	protected:
		Layer2D * m_layer;

	public:
		virtual void render( unsigned int _debugMask );
		virtual void _render( unsigned int _debugMask );
		bool isRenderable() const;

	public:
		void destroy();

		virtual void visit( Visitor * _visitor );
		virtual void visitChildren( Visitor * _visitor );

	public:
		const mt::mat3f & getWorldMatrix();
		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();


		virtual mt::vec2f getScreenPosition();

	public:
		void setParent( Node * _node );
		Node* getParent();
		bool addChildren( Node * _node );
		bool addChildrenFront( Node* _node );
		void removeChildren( Node * _node );
		virtual Node * getChildren( const String& _name, bool _recursion ) const;
		bool isChildren( Node * _node, bool _recursive ) const;
		virtual void _addChildren( Node * _node );
		virtual void _removeChildren( Node * _node );

	protected:
		typedef std::list<Node *> TContainerChildren;
		TContainerChildren m_children;

		Node * m_parent;

	protected:
		virtual void _destroy();
	
	public:
		bool activate();
		void deactivate();
		bool isActivate() const;

		////
		void setLocalColor( const ColourValue& _color );
		void setLocalAlpha( float _alpha );
		const ColourValue& getWorldColor();
		const ColourValue& getLocalColor() const;
		void invalidateColor();

		void localColorTo( float _time, const ColourValue& _color );
		void localColorToCb( float _time, const ColourValue& _color, PyObject* _cbStop, PyObject* _cbEnd);

		void localAlphaTo( float _time, float _alpha );

		void localColorToStop();
		void localColorToStopCb(PyObject* _cb);

		virtual void colorTo( const ColourValue& _color, float _time );
		virtual void setAlpha( float _alpha );
		virtual void alphaTo( float _alpha, float _time );
		virtual void colorToStop();

	protected:
		virtual bool _activate();
		virtual void _deactivate();

	public:
		virtual void hide( bool _value );
		bool isHide() const;

	public:
		bool isUpdatable() const;

	public:
		const mt::box2f & getBoundingBox();
		void invalidateBoundingBox();

	public:
		bool compile();
		void release();
		bool recompile();

	public:
		void enable();
		void disable();
		bool isEnable() const;

		void setUpdatable( bool _updatable );
		bool updatable() const;

		bool getUpdatable();

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
		virtual void _updateBoundingBox( mt::box2f& _boundingBox ) override;

	public:
		void moveTo( float _time, const mt::vec2f& _point );
		void moveToCb( float _time, const mt::vec2f& _point,PyObject* _cb );

		void moveToStop();
		void moveToStopCb(PyObject* _cb);

	protected:
		ValueInterpolator<mt::vec2f> m_moveTo;
		ColourValue m_colorLocal;
		ColourValue m_colorWorld;
		bool m_invalidateColor;
		ValueInterpolator<ColourValue> m_colorLocalTo;
	};
}
