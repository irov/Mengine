#	pragma once

#	include "Factorable.h"

#	include "Identity.h"
#	include "Scriptable.h"
#	include "Eventable.h"
#	include "Resource.h"
#	include "GlobalHandleAdapter.h"
#	include "Updatable.h"
#	include "Renderable.h"
#	include "BoundingBox.h"
#	include "Transformation3D.h"
#	include "Colorable.h"
#	include "Affectorable.h"

#	include "ValueInterpolator.h"
#	include "Vertex.h"

#	include "Core/Viewport.h"
#	include "Core/IntrusiveList.h"
#	include "Core/IntrusiveSlug.h"

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
	typedef IntrusiveSlug<Node> TSlugChild;

	class Node
		: public IntrusiveLinked
		, public Factorable
		, public Identity
		, public Resource
		, public GlobalHandleAdapter
		, public Updatable
		, public Renderable
		, public BoundingBox
		, public Transformation3D
		, public Colorable
		, public Affectorable
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
		void render( RenderCameraInterface * _camera ) override;	
		inline bool isRenderable() const;

	public:
		void setRenderCamera( RenderCameraInterface * _camera );
		RenderCameraInterface * getRenderCamera() const;

	protected:
		void _hide( bool _value ) override;
			
//#	ifndef MENGE_MASTER_RELEASE
	protected:
		void _debugRender( RenderCameraInterface* _camera, unsigned int _debugMask ) override;
//#	endif

	protected:
		RenderCameraInterface * m_renderCamera;

	protected:
		void renderChild( RenderCameraInterface * _camera );

	protected:
		bool isEnableGlobalHandle() const override;

	public:
		virtual void visit( Visitor * _visitor );
		virtual void visitChildren( Visitor * _visitor );

	public:
		mt::vec2f getCameraPosition( RenderCameraInterface * _camera );
		
	public:
		const ColourValue & getWorldColor() const;
		void calcTotalColor( ColourValue & _color ) const;

	protected:
		void _invalidateColor() override;

	protected:
		void setParent_( Node * _node );

	public:
		inline Node * getParent() const;
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
		Node * m_parent;

		TListChild m_child;		

	private:
		bool addChildren_( TListChild::iterator _insert, Node * _node );

		void insertChildren_( TListChild::iterator _insert, Node * _node );
		void eraseChildren_( TListChild::iterator _it );

	protected:
		bool _destroy() override;
		void _deferredDestroy() override;
	
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

	public:
		

	protected:
		void _invalidateWorldMatrix() override;
		//void _invalidateBoundingBox() override;

	protected:
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
		void update( float _current, float _timing ) override;

	protected:
		void _update( float _current, float _timing ) override;

	protected:
		void updateChild_( float _current, float _timing );
		
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
		bool m_rendering;

	protected:
		void updateRendering_();

	protected:
		void getBoundingBox( mt::box2f & _boundingBox );

	protected:
		void _updateBoundingBox( mt::box2f& _boundingBox ) override;

	protected:
		size_t m_cameraRevision;

	protected:
		bool m_inUpdate;

//#ifndef MENGE_MASTER_RELEASE
	protected:
		const RenderMaterial* m_debugMaterial;
		Vertex2D m_debugBox[5];
//#endif
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
	inline Node * Node::getParent() const
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
		return m_rendering;
	}
}
