#pragma once

#include "Kernel/Identity.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Compilable.h"
#include "Kernel/Updatable.h"
#include "Kernel/Renderable.h"
#include "Kernel/BoundingBox.h"
#include "Kernel/Transformation3D.h"
#include "Kernel/Colorable.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Visitable.h"
#include "Kernel/Servant.h"

#include "Factory/Factorable.h"

#include "Core/Viewport.h"

#include "stdex/intrusive_slug_list_size_ptr.h"
#include "stdex/intrusive_slug_ptr.h"

namespace Mengine
{		
    typedef IntrusivePtr<class MousePickerTrapInterface> MousePickerTrapInterfacePtr;
    typedef IntrusivePtr<class RenderCameraInterface> RenderCameraInterfacePtr;
    typedef IntrusivePtr<class RenderScissorInterface> RenderScissorInterfacePtr;
    typedef IntrusivePtr<class RenderViewportInterface> RenderViewportInterfacePtr;
    typedef IntrusivePtr<class RenderTargetInterface> RenderTargetInterfacePtr;
	
	typedef IntrusivePtr<class Node> NodePtr;

    typedef stdex::intrusive_slug_list_size_ptr<Node> TListNodeChild;
	typedef stdex::intrusive_slug_ptr<Node> TSlugChild;
    
	class Node
        : public stdex::intrusive_slug_linked_ptr<Node>
		, public Factorable
        , public Identity
		, public Compilable
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
		~Node() override;
        
	public:
		void render( const RenderContext * _state ) override;
		inline bool isRenderable() const;

    public:
        virtual void _renderTarget( const RenderContext * _state );
		
	public:
		void setRenderViewport( const RenderViewportInterfacePtr & _viewport );
		const RenderViewportInterfacePtr & getRenderViewport() const;

	public:
		const RenderViewportInterfacePtr & getRenderViewportInheritance() const;
		
	public:
		void setRenderCamera( const RenderCameraInterfacePtr & _camera );
        const RenderCameraInterfacePtr & getRenderCamera() const;

	public:
        const RenderCameraInterfacePtr & getRenderCameraInheritance() const;

	public:
		void setRenderScissor( const RenderScissorInterfacePtr & _scissor );
		const RenderScissorInterfacePtr & getRenderScissor() const;

	public:
		const RenderScissorInterfacePtr & getRenderScissorInheritance() const;

    public:
        void setRenderTarget( const RenderTargetInterfacePtr & _target );
        const RenderTargetInterfacePtr & getRenderTarget() const;

    public:
        const RenderTargetInterfacePtr & getRenderTargetInheritance() const;

	protected:
		void _setHide( bool _hide ) override;
        void _setExternalRender( bool _externalRender ) override;
			
	protected:
		void _debugRender( const RenderContext * _state ) override;
	
	protected:
		RenderViewportInterfacePtr m_renderViewport;
		RenderCameraInterfacePtr m_renderCamera;
		RenderScissorInterfacePtr m_renderScissor;
		
    protected:
        RenderTargetInterfacePtr m_renderTarget;

	protected:
		void renderChild_( const RenderContext * _state );

	public:
		void calcScreenPosition( const RenderCameraInterfacePtr & _camera, mt::vec2f & _screen );
		
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
		void addChild( const NodePtr & _node );
		void addChildFront( const NodePtr & _node );
		bool addChildAfter( const NodePtr & _node, const NodePtr & _after );
		bool removeChild( const NodePtr & _node );
		void removeChildren();
		bool removeFromParent();

		bool isHomeless() const;
		
		void destroyAllChild();

		inline TListNodeChild & getChildren();
		inline const TListNodeChild & getChildren() const;

        NodePtr findChild( const ConstString & _name, bool _recursion ) const;
        NodePtr getSiblingPrev();
        NodePtr getSiblingNext();
		bool hasChild( const ConstString & _name, bool _recursive ) const;
		bool emptyChildren() const;

    protected:
        void removeChild_( const NodePtr & _node );
		void setParent_( Node * _node );

	protected:
		virtual bool _hasChild( const ConstString & _name, bool _recursive ) const;
		virtual Node * _findChild( const ConstString & _name, bool _recursion ) const;

	protected:
		virtual void _changeParent( Node * _oldParent, Node * _newParent );
		virtual void _addChild( const NodePtr & _node );
		virtual void _removeChild( const NodePtr & _node );
		
	protected:
		Node * m_parent;

		TListNodeChild m_children;

	private:
		void addChild_( TListNodeChild::iterator _insert, const NodePtr & _node );

		void insertChild_( TListNodeChild::iterator _insert, const NodePtr & _node );
		void eraseChild_( const NodePtr & _node );

	public:
		void visitChildren( Visitor * _visitor );
        
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
		virtual MousePickerTrapInterfacePtr getPickerTrap();

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
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Node> NodePtr;
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
    struct reinterpret_node_cast_void_t
    {
        typedef void * type;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class U>
    struct reinterpret_node_cast_void_t<const U *>
    {
        typedef const void * type;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    inline T reinterpret_node_cast( void * _node )
    {
#ifndef NDEBUG
        if( _node == nullptr )
        {
            return nullptr;
        }

        try {
            if( dynamic_cast<typename reinterpret_node_cast_void_t<T>::type>(static_cast<T>(_node)) == nullptr )
            {
                throw;
            }
        }
        catch( const std::exception & )
        {
            throw;
        }
#endif

        return reinterpret_cast<T>(_node);
    }
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	inline T static_node_cast( Node * _node )
	{
#ifndef NDEBUG
        if( _node == nullptr )
        {
            return nullptr;
        }

		if( dynamic_cast<T>(_node) == nullptr )
		{
            throw;
		}
#endif

		return static_cast<T>(_node);
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	inline T static_node_cast( const Node * _node )
	{
#ifndef NDEBUG
        if( _node == nullptr )
        {
            return nullptr;
        }

		if( dynamic_cast<T>(_node) == nullptr )
		{
            throw;
		}
#endif

		return static_cast<T>(_node);
	}
}

