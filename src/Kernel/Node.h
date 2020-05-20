#pragma once

#include "Kernel/Identity.h"
#include "Kernel/Hierarchyable.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Pickerable.h"
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

#ifdef MENGINE_DEBUG
#include <type_traits>
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveSlugListSize<Node> IntrusiveSlugListNodeChild;
    typedef IntrusiveSlugIterator<IntrusiveSlugListNodeChild> IntrusiveSlugChild;
    //////////////////////////////////////////////////////////////////////////
    class Node
        : public stdex::intrusive_slug_linked_ptr<Node, void, IntrusivePtr, IntrusivePtrBase>
        , public Factorable
        , public Identity
        , public Compilable
        , public Hierarchyable
        , public Updatable
        , public Renderable
        , public Transformation
        , public Affectorable
        , public AffectorHubProviderInterface
        , public Visitable
        , public Scriptable
        , public Animatable
        , public Eventable
        , public Pickerable
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
        MENGINE_INLINE Node * getParent() const;
        MENGINE_INLINE bool hasParent() const;

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

        MENGINE_INLINE IntrusiveSlugListNodeChild & getChildren();
        MENGINE_INLINE const IntrusiveSlugListNodeChild & getChildren() const;
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
        void setParent_( Node * _parent );
        void refreshRenderRelation_( Node * _parent );
        void refreshPickerRelation_( Node * _parent );

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

    protected:
        void addChild_( const IntrusiveSlugListNodeChild::iterator & _insert, const NodePtr & _node );

    protected:
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

    public:
        void visitChildren( const VisitorPtr & _visitor );

    protected:
        void removeRelationRender_();

    protected:
        typedef const Lambda<void( RenderInterface * )> LambdaRenderCloseChildren;
        void foreachRenderCloseChildren_( const LambdaRenderCloseChildren & _lambda );
        void foreachRenderReverseCloseChildren_( const LambdaRenderCloseChildren & _lambda );

    protected:
        void removeRelationPicker_();

    protected:
        typedef const Lambda<void( PickerInterface * )> LambdaPickerCloseChildren;
        void foreachPickerCloseChildren_( const LambdaPickerCloseChildren & _lambda );
        void foreachPickerReverseCloseChildren_( const LambdaPickerCloseChildren & _lambda );

    protected:
        void _destroy() override;

    protected:
        bool activate();
        void deactivate();

    public:
        MENGINE_INLINE bool isActivate() const;
        MENGINE_INLINE bool isAfterActive() const;
        MENGINE_INLINE bool isDeactivating() const;

    protected:
        virtual bool _activate();
        virtual bool _afterActivate();

        virtual void _deactivate();
        virtual void _afterDeactivate();

    protected:
        void _invalidateWorldMatrix() const override;

    protected:
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
        MENGINE_INLINE bool isEnable() const;

    public:
        void freeze( bool _value );
        MENGINE_INLINE bool isFreeze() const;

    protected:
        virtual void _freeze( bool _value );

    public:
        void setSpeedFactor( float _speedFactor );
        float getSpeedFactor() const;

    protected:
        const AffectorHubProviderInterface * getAffectorHubProvider() const override;

    protected:
        EUpdateMode getAffectorableUpdatableMode() const override;
        uint32_t getAffectorableUpdatableLeafDeep() const override;

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
    MENGINE_INLINE bool Node::isActivate() const
    {
        return m_active;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isAfterActive() const
    {
        return m_afterActive;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isDeactivating() const
    {
        return m_deactivating;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isFreeze() const
    {
        return m_freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isEnable() const
    {
        return m_enable;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE Node * Node::getParent() const
    {
        return m_parent;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::hasParent() const
    {
        return m_parent != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE IntrusiveSlugListNodeChild & Node::getChildren()
    {
        return m_children;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const IntrusiveSlugListNodeChild & Node::getChildren() const
    {
        return m_children;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        namespace Detail
        {
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
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T reinterpretNodeCast( void * _node )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "reinterpret node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            try
            {
                if( dynamic_cast<typename Detail::reinterpret_node_cast_void_t<T>::type>(static_cast<T>(_node)) == nullptr )
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
        T staticNodeCast( Node * _node )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "static node cast use on non 'Nodeable' type");

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
        T staticNodeCast( const Node * _node )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "static node cast use on non 'Nodeable' type");

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
        T dynamicNodeCast( Node * _node )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "dynamic node cast use on non 'Nodeable' type");
#endif

            T t = dynamic_cast<T>(_node);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicNodeCast( const Node * _node )
        {            
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "dynamic node cast use on non 'Nodeable' type");
#endif

            T t = dynamic_cast<T>(_node);

            return t;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    using IntrusiveNodePtr = IntrusivePtr<T, Node>;
    //////////////////////////////////////////////////////////////////////////
}