#pragma once

#include "Kernel/Identity.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Compilable.h"
#include "Kernel/Updatable.h"
#include "Kernel/Renderable.h"
#include "Kernel/Transformation.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Visitable.h"
#include "Kernel/Animatable.h"
#include "Kernel/Unknowable.h"
#include "Kernel/Factorable.h"
#include "Kernel/IntrusiveSlugListSize.h"
#include "Kernel/IntrusiveSlugIterator.h"
#include "Kernel/Viewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ENodeChildInsertMode
    {
        ENCI_FRONT,
        ENCI_MIDDLE,
        ENCI_BACK
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class PickerTrapInterface> PickerTrapInterfacePtr;
    typedef IntrusivePtr<class Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveSlugListSize<Node> IntrusiveSlugListNodeChild;
    typedef IntrusiveSlugIterator<IntrusiveSlugListNodeChild> IntrusiveSlugChild;
    //////////////////////////////////////////////////////////////////////////
    class Node
        : public stdex::intrusive_slug_linked_ptr<Node, IntrusivePtr, IntrusivePtrBase>
        , public Factorable
        , public Identity
        , public Compilable
        , public Updatable
        , public Renderable
        , public Transformation
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

    public:
        void setUniqueIdentity( uint32_t _uniqueIdentity );
        uint32_t getUniqueIdentity() const;

    protected:
        uint32_t m_uniqueIdentity;

    public:
        inline Node * getParent() const;
        inline bool hasParent() const;

    public:
        void addChild( const NodePtr & _node );
        void addChildFront( const NodePtr & _node );
        bool addChildAfter( const NodePtr & _node, const NodePtr & _after );
        bool removeChild( const NodePtr & _node );

        typedef Lambda<void( const NodePtr & node )> LambdaRemoveChildren;
        void removeChildren( const LambdaRemoveChildren & _lambda );
        bool removeFromParent();

        typedef Lambda<void( const NodePtr & node )> LambdaDestroyChildren;
        void destroyChildren( const LambdaDestroyChildren & _lambda );

        inline IntrusiveSlugListNodeChild & getChildren();
        inline const IntrusiveSlugListNodeChild & getChildren() const;
        uint32_t getChildrenRecursiveCount() const;

        NodePtr findChild( const ConstString & _name, bool _recursion ) const;
        NodePtr getSiblingPrev() const;
        NodePtr getSiblingNext() const;
        bool hasChild( const ConstString & _name, bool _recursive ) const;
        bool emptyChildren() const;

    protected:
        uint32_t getLeafDeep() const;

    protected:
        void removeChild_( const NodePtr & _node );
        void removeParent_();
        void setParent_( Node * _node, ENodeChildInsertMode _mode );

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
        void addChild_( const IntrusiveSlugListNodeChild::iterator & _insert, const NodePtr & _node, ENodeChildInsertMode _mode );

        void insertChild_( const IntrusiveSlugListNodeChild::iterator & _insert, const NodePtr & _node );
        void eraseChild_( const NodePtr & _node );

    public:
        typedef Lambda<void( const NodePtr & )> LambdaNode;
        void foreachChildren( const LambdaNode & _lambda ) const;
        void foreachChildrenUnslug( const LambdaNode & _lambda ) const;
        void foreachChildrenReverse( const LambdaNode & _lambda ) const;

        typedef Lambda<bool( const NodePtr & )> LambdaNodeBreak;
        void foreachChildrenUnslugBreak( const LambdaNodeBreak & _lambda ) const;

        NodePtr findUniqueChild( uint32_t _uniqueIdentity ) const;

        void removeRelationRender_();
        void setRelationRender_( Node * _parent );
        void setRelationRenderFront_( Node * _parent );
        void moveChildRenderFront_( const NodePtr & _child );
        void moveChildRenderBack_( const NodePtr & _child );

        typedef const Lambda<void( RenderInterface * )> LambdaNodeRenderCloseChildren;
        void foreachRenderCloseChildren( const LambdaNodeRenderCloseChildren & _lambda );
        void foreachReverseRenderCloseChildren( const LambdaNodeRenderCloseChildren & _lambda );

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
        bool enableForce();

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

    protected:
        uint32_t getAffectorableUpdatableMode() const override;
        uint32_t getAffectorableUpdatableLeafDeep() const override;

    public:
        virtual PickerTrapInterface * getPickerTrap();

    protected:
        bool m_active;
        bool m_deactivating;
        bool m_afterActive;
        bool m_enable;

        bool m_freeze;
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
#ifdef MENGINE_DEBUG
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
    inline T node_static_cast( Node * _node )
    {
#ifdef MENGINE_DEBUG
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
    inline T node_static_cast( const Node * _node )
    {
#ifdef MENGINE_DEBUG
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

