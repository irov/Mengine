#pragma once

#include "Kernel/Identity.h"
#include "Kernel/Hierarchy.h"
#include "Kernel/Scriptable.h"
#include "Kernel/Pickerable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Compilable.h"
#include "Kernel/Updatable.h"
#include "Kernel/Renderable.h"
#include "Kernel/Transformable.h"
#include "Kernel/Affectorable.h"
#include "Kernel/Visitable.h"
#include "Kernel/Animatable.h"
#include "Kernel/Unknowable.h"
#include "Kernel/Factorable.h"
#include "Kernel/Viewport.h"

#ifdef MENGINE_DEBUG
#   include <type_traits>
#   include <stdexcept>
#endif

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    class Node
        : public Factorable
        , public Identity
        , public Compilable
        , public Hierarchy
        , public Affectorable
        , public AffectorHubProviderInterface
        , public Updatable
        , public Renderable
        , public Transformable
        , public Visitable
        , public Animatable
        , public Eventable
        , public Pickerable
        , public Unknowable
#ifdef MENGINE_USE_SCRIPT_SERVICE
        , public Scriptable
#endif
    {
        DECLARE_VISITABLE_BASE();

    public:
        Node();
        ~Node() override;

    public:
        NodePtr findUniqueChild( UniqueId _uniqueIdentity ) const;

    protected:
        void refreshRenderRelation_( Node * _parent );
        void removeRelationRender_();

    protected:
        typedef const Lambda<void( RenderInterface * )> LambdaRenderCloseChildren;
        void foreachRenderCloseChildren_( const LambdaRenderCloseChildren & _lambda );
        void foreachRenderReverseCloseChildren_( const LambdaRenderCloseChildren & _lambda );

    protected:
        void refreshPickerRelation_( Node * _parent );
        void removeRelationPicker_();

    protected:
        typedef const Lambda<void( PickerInterface * )> LambdaPickerCloseChildren;
        void foreachPickerCloseChildren_( const LambdaPickerCloseChildren & _lambda );
        void foreachPickerReverseCloseChildren_( const LambdaPickerCloseChildren & _lambda );

    protected:
        void visitChildren( const VisitorPtr & _visitor );

    public:
        void dispose();
        void disposeAll();

    protected:
        virtual void _dispose();

    public:
        MENGINE_INLINE bool isDispose() const;

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
        void _hierarchySetParent( Node * _newParent ) override;
        void _hierarchyRemoveParent( Node * _oldParent ) override;
        void _hierarchyChangeParent( Node * _oldParent, Node * _newParent ) override;
        void _hierarchyRefreshChild( const NodePtr & _node ) override;
        void _hierarchyAddChild( const NodePtr & _node ) override;
        void _hierarchyRemoveChild( const NodePtr & _node ) override;

    protected:
        AffectorHubProviderInterface * getAffectorHubProvider() override;

    protected:
        EUpdateMode getAffectorableUpdatableMode() const override;
        uint32_t getAffectorableUpdatableLeafDeep() const override;

    protected:
        bool m_active;
        bool m_deactivating;
        bool m_afterActive;
        bool m_enable;
        bool m_dispose;

        bool m_freeze;

    protected:
        friend class Hierarchy;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isDispose() const
    {
        return m_dispose;
    }
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

            if( dynamic_cast<typename Detail::reinterpret_node_cast_void_t<T>::type>(static_cast<T>(_node)) == nullptr )
            {
                throw std::runtime_error( "reinterpret node cast" );
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
                throw std::runtime_error( "static node cast" );
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
                throw std::runtime_error( "static node cast" );
            }
#endif

            T t = static_cast<T>(_node);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticNodeCast( const NodePtr & _node )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<typename T::value_type>>, "static node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            if( stdex::intrusive_dynamic_cast<T>(_node) == nullptr )
            {
                throw std::runtime_error( "static node cast" );
            }
#endif

            T t = stdex::intrusive_static_cast<T>(_node);

            return t;
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
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicNodeCast( const NodePtr & _node )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<typename T::value_type>>, "dynamic node cast use on non 'Nodeable' type");
#endif

            T t = stdex::intrusive_dynamic_cast<T>(_node);

            return t;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    using IntrusiveNodePtr = IntrusivePtr<T, Node>;
    //////////////////////////////////////////////////////////////////////////
}