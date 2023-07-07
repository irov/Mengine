#pragma once

#include "Kernel/IntrusiveSlugListSize.h"
#include "Kernel/IntrusiveSlugIterator.h"
#include "Kernel/Mixin.h"
#include "Kernel/Visitor.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class EHierarchyInsert
    {
        EHI_BACK,
        EHI_FRONT,
        EHI_UNKNOWN,
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveSlugListSize<Node> IntrusiveSlugListHierarchyChild;
    typedef IntrusiveSlugIterator<IntrusiveSlugListHierarchyChild> IntrusiveSlugIteratorHierarchyChild;
    //////////////////////////////////////////////////////////////////////////
    class Hierarchy
        : public IntrusiveSlugLinkedPtr<Node, void, IntrusivePtr, IntrusivePtrBase>
        , public Mixin
    {
    public:
        Hierarchy();
        ~Hierarchy() override;

    public:
        MENGINE_INLINE Node * getParent() const;
        MENGINE_INLINE bool hasParent() const;

    public:
        void addChild( const NodePtr & _node );
        void addChildFront( const NodePtr & _node );
        bool addChildAfter( const NodePtr & _node, const NodePtr & _after );
        bool removeChild( const NodePtr & _node );

        typedef Lambda<void( const NodePtr & _node )> LambdaRemoveChildren;
        void removeChildren( const LambdaRemoveChildren & _lambda );
        bool removeFromParent();

        uint32_t getChildrenCount() const;
        uint32_t getChildrenRecursiveCount() const;

        NodePtr getSiblingPrev() const;
        NodePtr getSiblingNext() const;

        bool isEmptyChildren() const;

    public:
        typedef Lambda<bool( const NodePtr & )> LambdaFindChild;
        bool hasChild( const LambdaFindChild & _lambda, bool _recursive ) const;
        NodePtr findChild( const LambdaFindChild & _lambda, bool _recursive ) const;

    protected:
        uint32_t getLeafDeep() const;

    protected:
        MENGINE_INLINE IntrusiveSlugListHierarchyChild & getChildren_();
        MENGINE_INLINE const IntrusiveSlugListHierarchyChild & getChildren_() const;

    protected:
        void removeChild_( const NodePtr & _node );
        void removeParent_();
        void setParent_( Node * _parent, EHierarchyInsert _hint );

    protected:
        void addChild_( const IntrusiveSlugListHierarchyChild::iterator & _insert, const NodePtr & _node, EHierarchyInsert _hint );

    protected:
        void insertChild_( const IntrusiveSlugListHierarchyChild::iterator & _insert, const NodePtr & _node );
        void eraseChild_( const NodePtr & _node );

    public:
        typedef Lambda<void( const NodePtr & )> LambdaHierarchy;
        void foreachChildren( const LambdaHierarchy & _lambda ) const;
        void foreachChildrenSlug( const LambdaHierarchy & _lambda ) const;
        void foreachChildrenUnslug( const LambdaHierarchy & _lambda ) const;
        void foreachChildrenReverse( const LambdaHierarchy & _lambda ) const;

        typedef Lambda<bool( const NodePtr & )> LambdaHierarchyBreak;
        bool foreachChildrenSlugBreak( const LambdaHierarchyBreak & _lambda ) const;
        bool foreachChildrenUnslugBreak( const LambdaHierarchyBreak & _lambda ) const;
        bool foreachChildrenReverseBreak( const LambdaHierarchyBreak & _lambda ) const;

    public:
        void foreachThree( const LambdaHierarchy & _lambda ) const;

    protected:
        virtual void _hierarchySetParent( Node * _newParent, EHierarchyInsert _hint ) = 0;
        virtual void _hierarchyRemoveParent( Node * _oldParent ) = 0;
        virtual void _hierarchyChangeParent( Node * _oldParent, Node * _newParent ) = 0;
        virtual void _hierarchyRefreshChild( const NodePtr & _hierarchy, EHierarchyInsert _hint ) = 0;
        virtual void _hierarchyAddChild( const NodePtr & _hierarchy ) = 0;
        virtual void _hierarchyRemoveChild( const NodePtr & _hierarchy ) = 0;

    protected:
        Node * m_parent;

        IntrusiveSlugListHierarchyChild m_children;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Hierarchy> HierarchyPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE Node * Hierarchy::getParent() const
    {
        return m_parent;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Hierarchy::hasParent() const
    {
        return m_parent != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE IntrusiveSlugListHierarchyChild & Hierarchy::getChildren_()
    {
        return m_children;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const IntrusiveSlugListHierarchyChild & Hierarchy::getChildren_() const
    {
        return m_children;
    }
    //////////////////////////////////////////////////////////////////////////
}