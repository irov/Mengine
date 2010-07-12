#	pragma once

#	include "Factory/Factorable.h"
#	include "Core/Viewport.h"

#	include "Loadable.h"
#	include "Identity.h"
#	include "Scriptable.h"
#	include "Eventable.h"
#	include "Resource.h"
#	include "Renderable.h"
#	include "InputHandler.h"
#	include "BoundingBox.h"
#	include "Transformation2D.h"
#	include "Colorable.h"
#	include "Affectorable.h"

#	include "ValueInterpolator.h"
#	include "Vertex.h"

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
		, virtual public Loadable
		, virtual public Scriptable
		, virtual public Renderable
		, virtual public Eventable
		, virtual public BoundingBox
		, virtual public Transformation2D
		, virtual public Colorable
		, virtual public Affectorable
	{
	public:
		Node();
		~Node();

	public:
		virtual void setLayer( Layer * _layer );
		Layer * getLayer() const;

	public:
		virtual Scene * getScene() const;

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
		const mt::mat3f & getWorldMatrix();

		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();

		virtual mt::vec2f getScreenPosition();

		const ColourValue & getWorldColor() const;

	public:
		void setParent( Node * _node );
		inline Node * getParent();

		bool addChildren( Node * _node );
		bool addChildrenFront( Node* _node );
		void removeChildren( Node * _node );

		virtual Node * getChildren( const String& _name, bool _recursion ) const;
		virtual bool isChildren( Node * _node, bool _recursive ) const;

	protected:
		Node * getChildrenIdentity_( std::size_t _nameIdentity, bool _recursion ) const;

	protected:
		virtual void _changeParent( Node * _parent );
		virtual void _addChildren( Node * _node );
		virtual void _removeChildren( Node * _node );

	protected:
		typedef std::list<Node *> TContainerChildren;
		TContainerChildren m_children;

		Node * m_parent;

	private:
		bool addChildren_( Node * _node, TContainerChildren::iterator _insert );

	public:
		bool registerSelfEvent( EEventName _name, const String & _method );

	protected:
		void _destroy() override;
	
	public:
		bool activate();
		void deactivate();
		inline bool isActivate() const;

	protected:
		virtual bool _activate();
		virtual void _deactivate();

	public:
		inline bool isUpdatable() const;

	public:
		void invalidateWorldMatrix() override;
		void invalidateColor() override;
		void invalidateBoundingBox() override;

	protected:		
		bool _checkVisibility( const Viewport & _viewport ) override;

	public:
		bool compile() override;
		void release() override;

	public:
		void enable();
		void disable();
		inline bool isEnable() const;

	protected:
		virtual void _enable();
		virtual void _disable();

	public:
		void setUpdatable( bool _updatable );
		inline bool updatable() const;

		bool getUpdatable() const;

	public:
		virtual void update( float _timing );

	public:
		void loader( XmlElement * xml ) override;

	public:
		void parser( BinParser * _parser ) override;

	protected:
		void parserNode_( BinParser * _parser );

	protected:
		PyObject * _embedded() override;

	public:
		void setListener( PyObject * _listener );

	protected:
		virtual void _setListener( PyObject * _listener );

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

	protected:
		std::size_t m_cameraRevision;

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
