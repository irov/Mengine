#pragma once

#include "Kernel/Identity.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Compilable.h"
#include "Kernel/Updatable.h"
#include "Kernel/Renderable.h"
#include "Kernel/BoundingBox.h"
#include "Kernel/Transformation.h"
#include "Kernel/Colorable.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Visitable.h"
#include "Kernel/Servant.h"
#include "Kernel/Animatable.h"
#include "Kernel/Unknowable.h"

#include "Kernel/Factorable.h"

#include "Kernel/Viewport.h"

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

    typedef stdex::intrusive_slug_list_size_ptr<Node> IntrusiveSlugListNodeChild;
    typedef stdex::intrusive_slug_ptr<IntrusiveSlugListNodeChild> IntrusiveSlugChild;

    class Node
        : public stdex::intrusive_slug_linked_ptr<Node>
        , public Factorable
        , public Identity
        , public Compilable
        , public Updatable
        , public Renderable
        , public BoundingBox
        , public Transformation
        , public Colorable
        , public Affectorable
        , public Visitable
        , public Scriptable
        , public Animatable
        , public Eventable
        , public Unknowable
    {
        DECLARE_VISITABLE_BASE();

    public:
        Node();
        ~Node() override;

    //public:
    //    inline bool isRenderable() const;

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

        void destroyAllChild();

        inline IntrusiveSlugListNodeChild & getChildren();
        inline const IntrusiveSlugListNodeChild & getChildren() const;
        uint32_t getChildrenRecursiveCount() const;

        NodePtr findChild( const ConstString & _name, bool _recursion ) const;
        NodePtr getSiblingPrev();
        NodePtr getSiblingNext();
        bool hasChild( const ConstString & _name, bool _recursive ) const;
        bool emptyChildren() const;

    protected:
        uint32_t getLeafDeep() const;

    protected:
        void removeChild_( const NodePtr & _node );
        void setParent_( Node * _node );

    protected:
        virtual bool _hasChild( const ConstString & _name, bool _recursive ) const;
        virtual NodePtr _findChild( const ConstString & _name, bool _recursion ) const;

    protected:
        virtual void _changeParent( Node * _oldParent, Node * _newParent );
        virtual void _addChild( const NodePtr & _node );
        virtual void _removeChild( const NodePtr & _node );

    protected:
        Node * m_parent;

        IntrusiveSlugListNodeChild m_children;

    private:
        void addChild_( IntrusiveSlugListNodeChild::iterator _insert, const NodePtr & _node );

        void insertChild_( IntrusiveSlugListNodeChild::iterator _insert, const NodePtr & _node );
        void eraseChild_( const NodePtr & _node );

    public:
        typedef Lambda<void( const NodePtr & )> LambdaNode;
        void foreachChildren( const LambdaNode & _lambda ) const;

    public:
        void visitChildren( const VisitorPtr & _visitor );

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
        void _invalidateWorldMatrix() override;
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
        void setHide( bool _hide );
        inline bool isHide() const;

    public:
        void setLocalHide( bool _localHide );
        inline bool isLocalHide() const;

    protected:
        virtual void _setHide( bool _hide );
        virtual void _setLocalHide( bool _localHide );

    protected:
        bool m_hide;
        bool m_localHide;

    public:
        void freeze( bool _value );
        inline bool isFreeze() const;

    protected:
        virtual void _freeze( bool _value );

    public:
        void setSpeedFactor( float _speedFactor );
        float getSpeedFactor() const;

    protected:
        uint32_t getAffectorableUpdatableMode() const override;
        uint32_t getAffectorableUpdatableLeafDeep() const override;

    public:
        virtual MousePickerTrapInterfacePtr getPickerTrap();

    protected:
        bool m_active;
        bool m_deactivating;
        bool m_afterActive;
        bool m_enable;

        bool m_freeze;

    //protected:
    //    mutable bool m_invalidateRendering;
    //    mutable bool m_rendering;

    //protected:
    //    void updateRendering_() const;

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
    inline bool Node::isHide() const
    {
        return m_hide;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool Node::isLocalHide() const
    {
        return m_localHide;
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
    ////////////////////////////////////////////////////////////////////////////
    //inline bool Node::isRenderable() const
    //{
    //    if( m_invalidateRendering == true )
    //    {
    //        this->updateRendering_();
    //    }

    //    return m_rendering;
    //}
    //////////////////////////////////////////////////////////////////////////
    inline IntrusiveSlugListNodeChild & Node::getChildren()
    {
        return m_children;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const IntrusiveSlugListNodeChild & Node::getChildren() const
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

