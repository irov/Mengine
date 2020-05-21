#pragma once

#include "Interface/HierarchyInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class BaseHierarchy, HierarchyInterface> BaseHierarchyPtr;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveSlugListSize<class BaseHierarchy, HierarchyInterface> IntrusiveSlugListHierarchyChild;
    typedef IntrusiveSlugIterator<IntrusiveSlugListHierarchyChild> IntrusiveSlugHierarchyChild;
    //////////////////////////////////////////////////////////////////////////
    class BaseHierarchy
        : public stdex::intrusive_slug_linked_ptr<class BaseHierarchy, HierarchyInterface, IntrusivePtr, IntrusivePtrBase>
        , public HierarchyInterface
    {
    public:
        BaseHierarchy();
        ~BaseHierarchy() override;

    public:
        MENGINE_INLINE BaseHierarchy * getParent() const override;
        MENGINE_INLINE bool hasParent() const override;

    public:
        void addChild( const HierarchyInterfacePtr & _node ) override;
        void addChildFront( const HierarchyInterfacePtr & _node ) override;
        bool addChildAfter( const HierarchyInterfacePtr & _node, const HierarchyInterfacePtr & _after ) override;
        bool removeChild( const HierarchyInterfacePtr & _node ) override;

        void removeChildren( const LambdaRemoveChildren & _lambda ) override;
        bool removeFromParent() override;

        void destroyChildren( const LambdaDestroyChildren & _lambda ) override;

        uint32_t getChildrenRecursiveCount() const override;

        HierarchyInterfacePtr getSiblingPrev() const override;
        HierarchyInterfacePtr getSiblingNext() const override;

        bool emptyChildren() const override;

    public:
        bool hasChild( const LambdaFindChild & _lambda, bool _recursive ) const override;
        HierarchyInterfacePtr findChild( const LambdaFindChild & _lambda, bool _recursive ) const override;

    protected:
        uint32_t getLeafDeep() const override;

    protected:
        MENGINE_INLINE IntrusiveSlugListHierarchyChild & getChildren_();
        MENGINE_INLINE const IntrusiveSlugListHierarchyChild & getChildren_() const;

    protected:
        void removeChild_( const BaseHierarchyPtr & _node );
        void removeParent_();
        void setParent_( BaseHierarchy * _parent );

    protected:
        BaseHierarchy * m_parent;

        IntrusiveSlugListHierarchyChild m_children;

    protected:
        void addChild_( const IntrusiveSlugListHierarchyChild::iterator & _insert, const BaseHierarchyPtr & _node );

    protected:
        void insertChild_( const IntrusiveSlugListHierarchyChild::iterator & _insert, const BaseHierarchyPtr & _node );
        void eraseChild_( const BaseHierarchyPtr & _node );

    public:
        void foreachChildren( const LambdaNode & _lambda ) const override;
        void foreachChildrenUnslug( const LambdaNode & _lambda ) const override;
        void foreachChildrenReverse( const LambdaNode & _lambda ) const override;
        void foreachChildrenUnslugBreak( const LambdaNodeBreak & _lambda ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE BaseHierarchy * BaseHierarchy::getParent() const
    {
        return m_parent;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseHierarchy::hasParent() const
    {
        return m_parent != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE IntrusiveSlugListHierarchyChild & BaseHierarchy::getChildren_()
    {
        return m_children;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const IntrusiveSlugListHierarchyChild & BaseHierarchy::getChildren_() const
    {
        return m_children;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BaseHierarchy, HierarchyInterface> BaseHierarchyPtr;
    //////////////////////////////////////////////////////////////////////////
}