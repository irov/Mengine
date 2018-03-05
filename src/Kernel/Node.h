#	pragma once

#	include "Kernel/Identity.h"
#	include "Kernel/Scriptable.h"
#	include "Kernel/Eventable.h"
#	include "Kernel/Resource.h"
//#	include "Kernel/GlobalHandleAdapter.h"
#	include "Kernel/Updatable.h"
#	include "Kernel/Renderable.h"
#	include "Kernel/BoundingBox.h"
#	include "Kernel/Transformation3D.h"
#	include "Kernel/Colorable.h"
#	include "Kernel/Affectorable.h"
#	include "Kernel/Visitable.h"
#	include "Kernel/Servant.h"

#	include "Factory/Factorable.h"

#	include "Core/Viewport.h"

#   include "stdex/intrusive_slug_list_size.h"
#   include "stdex/intrusive_slug.h"

namespace Menge
{		
	class MousePickerTrapInterface;

    class RenderViewportInterface;
    class RenderCameraInterface;
    class RenderClipplaneInterface;

    typedef stdex::intrusive_ptr<class RenderTargetInterface> RenderTargetInterfacePtr;
	
	class Node;

    typedef stdex::intrusive_slug_list_size<Node> TListNodeChild;
	typedef stdex::intrusive_slug<TListNodeChild> TSlugChild;

	class Node
        : public stdex::intrusive_slug_linked<Node>
		, public Factorable
        , public Identity
		, public Resource
		, public Updatable
		, public Renderable
		, public BoundingBox
		, public Transformation3D
		, public Colorable
		, public Affectorable
        , public Visitable
		, public Scriptable
	{
		DECLARE_VISITABLE_BASE();

	public:
		Node();
		~Node();
        
	public:
		void render( RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask ) override;
		inline bool isRenderable() const;

    public:
        virtual void _renderTarget( RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask );
		
	public:
		void setRenderViewport( const RenderViewportInterface * _viewport );
		const RenderViewportInterface * getRenderViewport() const;

	public:
		const RenderViewportInterface * getRenderViewportInheritance() const;
		
	public:
		void setRenderCamera( const RenderCameraInterface * _camera );
		const RenderCameraInterface * getRenderCamera() const;

	public:
		const RenderCameraInterface * getRenderCameraInheritance() const;

	public:
		void setRenderClipplane( const RenderClipplaneInterface * _clipplane );
		const RenderClipplaneInterface * getRenderClipplane() const;

	public:
		const RenderClipplaneInterface * getRenderClipplaneInheritance() const;

    public:
        void setRenderTarget( const RenderTargetInterfacePtr & _clipplane );
        const RenderTargetInterfacePtr & getRenderTarget() const;

    public:
        const RenderTargetInterfacePtr & getRenderTargetInheritance() const;

	protected:
		void _setHide( bool _hide ) override;
        void _setExternalRender( bool _externalRender ) override;
			
	protected:
		void _debugRender( RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask ) override;
	
	protected:
		const RenderViewportInterface * m_renderViewport;
		const RenderCameraInterface * m_renderCamera;
		const RenderClipplaneInterface * m_renderClipplane;
		
    protected:
        RenderTargetInterfacePtr m_renderTarget;

	protected:
		void renderChild_( RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask );

	public:
		void calcScreenPosition( const RenderCameraInterface * _camera, mt::vec2f & _screen );
		
	public:
		const ColourValue & getWorldColor() const;
		void calcTotalColor( ColourValue & _color ) const;
        bool isSolidColor() const;

	protected:
		void _invalidateColor() override;

	public:
		inline Node * getParent() const;
		inline bool hasParent() const;

    public:
		void addChild( Node * _node );
		void addChildFront( Node* _node );
		bool addChildAfter( Node* _node, Node * _after );
		bool removeChild( Node * _node );
		void removeChildren();
		bool removeFromParent();

		bool isHomeless() const;
		
		void destroyAllChild();

		inline TListNodeChild & getChildren();
		inline const TListNodeChild & getChildren() const;

		Node * findChild( const ConstString & _name, bool _recursion ) const;
        Node * getSiblingPrev();
        Node * getSiblingNext();
		bool hasChild( const ConstString & _name, bool _recursive ) const;
		bool emptyChildren() const;

    protected:
        void removeChild_( Node * _node );
		void setParent_( Node * _node );

	protected:
		virtual bool _hasChild( const ConstString & _name, bool _recursive ) const;
		virtual Node * _findChild( const ConstString & _name, bool _recursion ) const;

	protected:
		virtual void _changeParent( Node * _oldParent, Node * _newParent );
		virtual void _addChild( Node * _node );
		virtual void _removeChild( Node * _node );
		
	protected:
		Node * m_parent;

		TListNodeChild m_children;

	private:
		void addChild_( TListNodeChild::iterator _insert, Node * _node );

		void insertChild_( TListNodeChild::iterator _insert, Node * _node );
		void eraseChild_( Node * _node );

	public:
		void visitChildren( Visitor * _visitor );

	public:
		void destroy() override;

	protected:
		void _destroy() override;
	
	protected:
		bool activate();
		void deactivate();

	public:
		inline bool isActivate() const;
		inline bool isAfterActive() const;
		inline bool isDeactivating() const;

	protected:
		virtual bool _activate();
		virtual void _afterActivate();

		virtual void _deactivate();
		virtual void _afterDeactivate();

	protected:
		void _invalidateWorldMatrix () override;
		//void _invalidateBoundingBox() override;

	public:
		bool compile() override;
		void release() override;

	protected:
		void _recompile() override;
        void _uncompile() override;

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
		void setSpeedFactor( float _speedFactor );
		float getSpeedFactor() const;
	
	public:
		void update( float _current, float _timing ) override;

	protected:
		void _update( float _current, float _timing ) override;

	protected:
		void updateChildren_( float _current, float _timing );
		
	public:
		virtual MousePickerTrapInterface * getPickerTrap();

	protected:
		bool m_active;
		bool m_deactivating;
		bool m_afterActive;
		bool m_enable;

		float m_speedFactor;

		bool m_freeze;

	protected:
		mutable bool m_invalidateRendering;
		mutable bool m_rendering;

	protected:
		void updateRendering_() const;

	protected:
		void _updateBoundingBox( mt::box2f& _boundingBox ) const override;

	public:
		void setShallowGrave(); //Shallow Grave prevents any damage from killing the targetted hero. 
		void removeShallowGrave();

		bool isShallowGrave() const;

		void setShallowGravePropagate( bool _propagate );

	protected:
		virtual void _unshallowGrave();

	protected:
		int m_shallowGrave;
		bool m_shallowGravePropagate;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isActivate() const
	{
		return m_active;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isAfterActive() const
	{
		return m_afterActive;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isDeactivating() const
	{
		return m_deactivating;
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
		return m_parent != nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Node::isRenderable() const
	{
		if( m_invalidateRendering == true )
		{
			this->updateRendering_();
		}

		return m_rendering;
	}
	//////////////////////////////////////////////////////////////////////////
	inline TListNodeChild & Node::getChildren()
	{
		return m_children;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const TListNodeChild & Node::getChildren() const
	{
		return m_children;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	inline T static_node_cast( Node * _node )
	{
#	ifdef _DEBUG
		if( dynamic_cast<T>(_node) == nullptr )
		{
            throw;
		}
#	endif

		return static_cast<T>(_node);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	inline T static_node_cast( const Node * _node )
	{
#	ifdef _DEBUG
		if( dynamic_cast<T>(_node) == nullptr )
		{
            throw;
		}
#	endif

		return static_cast<T>(_node);
	}
}

