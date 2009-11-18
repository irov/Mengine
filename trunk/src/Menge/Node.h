#	pragma once

#	include "Factorable.h"

#	include "Identity.h"
#	include "Scriptable.h"
#	include "Eventable.h"
#	include "Resource.h"
#	include "Renderable.h"
#	include "InputHandler.h"
#	include "BoundingBox.h"
#	include "ObjectDeclare.h"
#	include "Core/Viewport.h"
#	include "Allocator2D.h"
#	include "Core/ColourValue.h"
#	include "ValueInterpolator.h"
#	include "Vertex.h"

class XmlElement;

#	define MENGE_AFFECTOR_POSITION		1
#	define MENGE_AFFECTOR_ANGLE			2
#	define MENGE_AFFECTOR_SCALE			3
#	define MENGE_AFFECTOR_COLOR			4
#	define MENGE_AFFECTOR_VISIBILITY	5

namespace Menge
{	
	class Visitor;
	
	class Layer2D;
	struct Material;

	class NodeAffector;	

	class Node
		: public Factorable
		, public Identity
		, public Resource
		, public Scriptable
		, public Renderable
		, public Eventable
		, public BoundingBox
		, public Allocator2D
		, public InputHandler
	{
	public:
		Node();
		~Node();

	protected:
		bool _checkVisibility( const Viewport & _viewport ) override;

	public:
		void setLayer( Layer2D * _layer );
		Layer2D * getLayer() const;

	public:
		void invalidateWorldMatrix();
		void _invalidateWorldMatrix() override;

	protected:
		Layer2D * m_layer;

	public:
		void render( Camera2D * _camera ) override;	
		void _render( Camera2D * _camera ) override;

		bool isRenderable() const;

	protected:
		void renderChild( Camera2D * _camera );

	public:
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
		bool addChildren_( Node * _node, TContainerChildren::iterator _insert );

	protected:
		TContainerChildren m_children;

		Node * m_parent;

	public:
		bool registerEvent( EEventName _name, const String & _method );

	protected:
		void _destroy() override;
	
	public:
		bool activate();
		void deactivate();
		inline bool isActivate() const;

		////
		void setLocalColor( const ColourValue& _color );
		void setLocalAlpha( float _alpha );

		const ColourValue & getWorldColor() const;
		inline const ColourValue & getLocalColor() const;

		void invalidateColor();
		virtual void _invalidateColor();

	protected:
		virtual bool _activate();
		virtual void _deactivate();

	public:
		inline bool isUpdatable() const;

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
		inline bool isEnable() const;

		void setUpdatable( bool _updatable );
		bool updatable() const;

		bool getUpdatable() const;

	public:
		virtual void update( float _timing );
		virtual void loader( XmlElement * xml );

	protected:
		PyObject * _embedded() override;

	public:
		void setListener( PyObject * _listener );
		PyObject* getListener();

	protected:
		virtual void _setListener();

	protected:
		PyObject * m_listener;

	protected:		
		bool m_active;
		bool m_enable;
		bool m_updatable;

		enum NodeState
		{
			NODE_IDLE,
			NODE_UPDATING,
			NODE_DEACTIVATING
		};

		NodeState m_state;

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
		ColourValue m_colorLocal;
		
		mutable ColourValue m_colorWorld;
		mutable bool m_invalidateColor;

		//typedef std::veco<NodeAffector*> TAffectorList;
		typedef std::vector<NodeAffector*> TAffectorVector;
		TAffectorVector m_affectorListToProcess;
		//typedef std::vector<NodeAffector*> TAffectorVector;
		TAffectorVector m_affectorsToAdd;

		float m_angularSpeed;
		mt::vec2f m_linearSpeed;

		void stopAffectors_( int _type );

#ifndef MENGE_MASTER_RELEASE
	protected:
		Material* m_debugMaterial;
		Vertex2D m_debugBox[4];
#endif
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isActivate() const
	{
		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ColourValue& Node::getLocalColor() const
	{
		return m_colorLocal;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isUpdatable() const
	{
		/*if( m_updatable == false )
		{
		return false;
		}*/

		if( m_enable == false )
		{
			return false;
		}

		if( m_active == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isEnable() const
	{
		return m_enable;
	}
}
