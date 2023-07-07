#pragma once

#include "Kernel/Identity.h"
#include "Kernel/Hierarchy.h"
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

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Kernel/Scriptable.h"
#endif

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    enum ENodeState
    {
        EN_STATE_ACTIVE = 1 << 0,
        EN_STATE_DEACTIVATING = 1 << 1,
        EN_STATE_AFTER_ACTIVE = 1 << 2,
        EN_STATE_ENABLE = 1 << 3,
        EN_STATE_DISPOSE = 1 << 4,
        EN_STATE_FREEZE = 1 << 5,
    };
    //////////////////////////////////////////////////////////////////////////
    class Node
        : public Factorable
        , public Identity
        , public Compilable
        , public Hierarchy
        , public Visitable
        , public Affectorable
        , public AffectorHubProviderInterface
        , public Updatable
        , public Renderable
        , public Transformable        
        , public Animatable
        , public Eventable
        , public Pickerable
        , public Unknowable
#if defined(MENGINE_USE_SCRIPT_SERVICE)
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
        void addTransformationRelation_( Node * _parent, EHierarchyInsert _hint );
        void refreshTransformationRelation_( EHierarchyInsert _hint );
        void removeTransformationRelation_();

    protected:
        void addRenderRelation_( Node * _parent, EHierarchyInsert _hint );
        void refreshRenderRelation_( EHierarchyInsert _hint );
        void removeRenderRelation_();

    protected:
        typedef Lambda<void( RenderInterface * )> LambdaRenderCloseChildren;
        void foreachRenderCloseChildren_( const LambdaRenderCloseChildren & _lambda );
        void foreachRenderReverseCloseChildren_( const LambdaRenderCloseChildren & _lambda );

    protected:
        void addPickerRelation_( Node * _parent, EHierarchyInsert _hint );
        void refreshPickerRelation_( EHierarchyInsert _hint );
        void removePickerRelation_();

    protected:
        typedef Lambda<void( PickerInterface * )> LambdaPickerCloseChildren;
        void foreachPickerCloseChildren_( const LambdaPickerCloseChildren & _lambda );
        void foreachPickerReverseCloseChildren_( const LambdaPickerCloseChildren & _lambda );

    public:
        void visitChildren( const VisitorPtr & _visitor );
        void visitThree( const VisitorPtr & _visitor );

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
        void _hierarchySetParent( Node * _newParent, EHierarchyInsert _hint ) override;
        void _hierarchyRemoveParent( Node * _oldParent ) override;
        void _hierarchyChangeParent( Node * _oldParent, Node * _newParent ) override;
        void _hierarchyRefreshChild( const NodePtr & _node, EHierarchyInsert _hint ) override;
        void _hierarchyAddChild( const NodePtr & _node ) override;
        void _hierarchyRemoveChild( const NodePtr & _node ) override;

    protected:
        AffectorHubProviderInterface * getAffectorHubProvider() override;

    protected:
        EUpdateMode getAffectorableUpdatableMode() const override;
        uint32_t getAffectorableUpdatableLeafDeep() const override;

    protected:
        uint32_t m_state;

    protected:
        friend class Hierarchy;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isDispose() const
    {
        return m_state & EN_STATE_DISPOSE;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isActivate() const
    {
        return m_state & EN_STATE_ACTIVE;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isAfterActive() const
    {
        return m_state & EN_STATE_AFTER_ACTIVE;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isDeactivating() const
    {
        return m_state & EN_STATE_DEACTIVATING;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isFreeze() const
    {
        return m_state & EN_STATE_FREEZE;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Node::isEnable() const
    {
        return m_state & EN_STATE_ENABLE;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    using IntrusiveNodePtr = IntrusivePtr<T, Node>;
    //////////////////////////////////////////////////////////////////////////
}