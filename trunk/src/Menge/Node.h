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
	
	class Layer;
	class Scene;

	struct Material;

	class Affector;	

	class Node
		: public Factorable
		, public Identity
		, public Resource
		, virtual public Scriptable
		, virtual public Renderable
		, virtual public Eventable
		, virtual public BoundingBox
		, virtual public Allocator2D
	{
	public:
		Node();
		~Node();

	protected:
		bool _checkVisibility( const Viewport & _viewport ) override;

	public:
		virtual void setLayer( Layer * _layer );
		Layer * getLayer() const;

	public:
		virtual Scene * getScene() const;

	public:
		void _invalidateWorldMatrix() override;

	protected:
		Layer * m_layer;

	public:
		void render( Camera2D * _camera ) override;	
		bool isRenderable() const;

	protected:
#	ifndef MENGE_MASTER_RELEASE
		void _debugRender( Camera2D* _camera, unsigned int _debugMask ) override;
#	endif

	protected:
		void renderChild( Camera2D * _camera );

	public:
		virtual void visit( Visitor * _visitor );
		virtual void visitChildren( Visitor * _visitor );

	public:
		const mt::mat3f & getWorldMatrix() override;

		virtual mt::vec2f getScreenPosition();

	public:
		void setParent( Node * _node );
		inline Node * getParent();

		bool addChildren( Node * _node );
		bool addChildrenFront( Node* _node );
		void removeChildren( Node * _node );

		virtual Node * getChildren( const String& _name, bool _recursion ) const;
		bool isChildren( Node * _node, bool _recursive ) const;

		virtual void _changeParent( Node * _parent );

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

		inline const ColourValue & getWorldColor() const;
		inline const ColourValue & getLocalColor() const;

	protected:
		void updateWorldColor() const;

		void invalidateColor();
		virtual void _invalidateColor();

	protected:
		virtual bool _activate();
		virtual void _deactivate();

	public:
		inline bool isUpdatable() const;

	public:
		void _invalidateBoundingBox() override;

	public:
		bool compile() override;
		void release() override;

	public:
		void enable();
		void disable();
		inline bool isEnable() const;

		virtual void _enable();
		virtual void _disable();

		void setUpdatable( bool _updatable );
		inline bool updatable() const;

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
		virtual void _postUpdate( float _timing );

	protected:
		void updateBoundingBox() override;
		void _updateBoundingBox( mt::box2f& _boundingBox ) override;

	public:
		void addAffector( Affector* _affector );

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

		std::size_t m_cameraRevision;

		//typedef std::veco<Affector*> TAffectorList;
		typedef std::vector<Affector*> TAffectorVector;
		TAffectorVector m_affectorListToProcess;
		//typedef std::vector<Affector*> TAffectorVector;
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
	const ColourValue & Node::getWorldColor() const
	{
		if( m_invalidateColor == true )
		{
			updateWorldColor();
		}

		return m_colorWorld;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ColourValue& Node::getLocalColor() const
	{
		return m_colorLocal;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::updatable() const
	{
		return m_updatable;
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
	//////////////////////////////////////////////////////////////////////////
	inline Node * Node::getParent()
	{
		return m_parent;
	}
}
