#	pragma once

#	include "Factorable.h"

#	include "Loadable.h"
#	include "Identity.h"
#	include "Scriptable.h"
#	include "Eventable.h"
#	include "Resource.h"
#	include "GlobalHandleAdapter.h"
#	include "Renderable.h"
#	include "BoundingBox.h"
#	include "Transformation2D.h"
#	include "Colorable.h"
#	include "Affectorable.h"

#	include "ValueInterpolator.h"
#	include "Vertex.h"

#	include "Core/Viewport.h"
#	include "Core/IntrusiveList.h"

namespace Menge
{	
	class Visitor;
	
	class Layer;
	class Scene;

	struct RenderMaterial;

	class Affector;

	class ResourceReference;

	class VisitorResource
	{
	public:
		virtual void visitResource( ResourceReference * _resource ) = 0;
	};

	class Node;

	typedef IntrusiveList<Node> TListChild;

	class Node
		: public IntrusiveLinked
		, public Factorable
		, public Identity
		, public Resource
		, public GlobalHandleAdapter
		, public Renderable
		, public BoundingBox
		, public Transformation2D
		, public Colorable
		, public Affectorable
		, virtual public Loadable
		, virtual public Scriptable
		, virtual public Eventable
	{
	public:
		Node();
		~Node();

	public:
		virtual void setLayer( Layer * _layer );
		Layer * getLayer() const;

	public:
		virtual Scene * getScene();

	protected:
		Layer * m_layer;

	public:
		void render( Camera2D * _camera ) override;	
		inline bool isRenderable() const;

	protected:
#	ifndef MENGE_MASTER_RELEASE
		void _debugRender( Camera2D* _camera, unsigned int _debugMask ) override;
#	endif

	protected:
		void renderChild( Camera2D * _camera );

	protected:
		bool isEnableGlobalHandle() const override;

	public:
		virtual void visit( Visitor * _visitor );
		virtual void visitChildren( Visitor * _visitor );

	public:
		const mt::mat3f & getWorldMatrix();

		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();

		mt::vec2f getCameraPosition( Camera2D * _camera2D );

		void setWorldPosition( const mt::vec2f & _pos );

	public:
		const ColourValue & getWorldColor() const;
		void calcTotalColor( ColourValue & _color ) const;

	protected:
		void _invalidateColor() override;

	protected:
		void setParent_( Node * _node );

	public:
		inline Node * getParent();
		inline bool hasParent() const;

		bool addChildren( Node * _node );
		bool addChildrenFront( Node* _node );
		bool addChildrenAfter( Node* _node, Node * _after );
		void removeChildren( Node * _node );
		void removeAllChild();
		void removeFromParent();
		
		void destroyAllChild();

		const TListChild & getChild() const;

		Node * findChildren( const ConstString & _name, bool _recursion ) const;
		Node * findTag( const ConstString & _tag ) const;
		bool hasChildren( const ConstString & _name, bool _recursive ) const;
		bool emptyChild() const;

	protected:
		virtual bool _hasChildren( const ConstString & _name, bool _recursive ) const;
		virtual Node * _findChildren( const ConstString & _name, bool _recursion ) const;

	protected:
		virtual void _changeParent( Node * _oldParent, Node * _newParent );
		virtual void _addChildren( Node * _node );
		virtual void _removeChildren( Node * _node );

	protected:
		TListChild m_child;

		Node * m_parent;

	private:
		bool addChildren_( Node * _node, TListChild::iterator _insert );

	protected:
		void _destroy() override;
	
	private:
		bool activate();
		void deactivate();

	public:
		inline bool isActivate() const;

	protected:
		virtual bool _activate();
		virtual void _afterActivate();

		virtual void _deactivate();
		virtual void _afterDeactivate();

	protected:
		void _invalidateWorldMatrix() override;
		void _invalidateBoundingBox() override;
		bool _checkVisibility( const Viewport & _viewport ) override;

	public:
		bool compile() override;
		void release() override;

	protected:
		void _recompile() override;

	public:
		bool enable();
		void disable();

	public:
		inline bool isEnable() const;

	public:
		void freeze( bool _value );
		inline bool isFreeze() const;

	protected:
		virtual void _freeze( bool _value );

	public:
		virtual void update( float _timing );

	protected:
		virtual void _update( float _timing );
		virtual void _postUpdate( float _timing );

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void _loaded() override;

	protected:
		PyObject * _embedded() override;
		void _embedding( PyObject * _embed ) override;

	public:
		PyObject * setEventListener( PyObject * _args, PyObject * _kwds );
		void removeEventListener();

	protected:
		virtual void _setEventListener( PyObject * _listener );

	public:
		virtual void visitResource( VisitorResource * _visitor );

	protected:
		bool m_active;
		bool m_enable;

		bool m_freeze;

	protected:
		void updateBoundingBox() override;
		void _updateBoundingBox( mt::box2f& _boundingBox ) override;

	protected:
		size_t m_cameraRevision;

#ifndef MENGE_MASTER_RELEASE
	protected:
		const RenderMaterial* m_debugMaterial;
		Vertex2D m_debugBox[5];
#endif
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isActivate() const
	{
		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isFreeze() const
	{
		return m_freeze;
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
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::hasParent() const
	{
		return m_parent != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isRenderable() const
	{
		if( isCompile() == false )
		{
			return false;
		}

		if( isActivate() == false )
		{
			return false;
		}

		if( isHide() == true )
		{
			return false;
		}

		return true;
	}
}
