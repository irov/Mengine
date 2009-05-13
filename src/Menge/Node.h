#	pragma once

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
#	include "ValueInterpolator.h"

class XmlElement;

#	define MENGE_AFFECTOR_POSITION 1
#	define MENGE_AFFECTOR_ANGLE 2
#	define MENGE_AFFECTOR_SCALE 3
#	define MENGE_AFFECTOR_COLOR 4

namespace Menge
{	
	class Visitor;
	class Layer2D;	

	class RenderObject;

	class NodeAffector;

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
		ColourValue& getWorldColor();
		const ColourValue& getLocalColor() const;
		void invalidateColor();

		virtual void setAlpha( float _alpha );

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
		void addAffector( NodeAffector* _affector );

		void moveToCb( float _time, const mt::vec2f& _point, PyObject* _cb );
		void moveToStop();

		void angleToCb( float _time, float _angle, PyObject* _cb );
		void angleToStop();

		void scaleToCb( float _time, const mt::vec2f& _scale, PyObject* _cb );
		void scaleToStop();

		void accMoveToCb( float _time, const mt::vec2f& _point, PyObject* _cb );
		void accAngleToCb( float _time, float _angle, PyObject* _cb );

		void localColorToCb( float _time, const ColourValue& _color, PyObject* _cb );
		void localAlphaToCb( float _time, float _alpha, PyObject* _cb );
		void localColorToStop();

	protected:
		ValueInterpolatorLinear<mt::vec2f> m_moveTo;
		ColourValue m_colorLocal;
		ColourValue m_colorWorld;
		bool m_invalidateColor;

		RenderObject* m_debugRenderObject;

		typedef std::list<NodeAffector*> TAffectorList;
		TAffectorList m_affectorListToProcess;
		typedef std::vector<NodeAffector*> TAffectorVector;
		TAffectorVector m_affectorsToAdd;

		float m_angularSpeed;
		mt::vec2f m_linearSpeed;

		void stopAffectors_( int _type );
	};
}
