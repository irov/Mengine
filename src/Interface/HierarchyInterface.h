#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/IntrusiveSlugListSize.h"
#include "Kernel/IntrusiveSlugIterator.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class HierarchyInterface> HierarchyInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class HierarchyReceiverInterface
        : public Mixin
    {
    public:
        virtual void onHierarchySetParent( HierarchyInterface * _newParent ) = 0;
        virtual void onHierarchyRemoveParent( HierarchyInterface * _oldParent ) = 0;
        virtual void onHierarchyChangeParent( HierarchyInterface * _oldParent, HierarchyInterface * _newParent ) = 0;
        virtual void onHierarchyRefreshChild( const HierarchyInterfacePtr & _hierarchy ) = 0;
        virtual void onHierarchyAddChild( const HierarchyInterfacePtr & _hierarchy ) = 0;
        virtual void onHierarchyRemoveChild( const HierarchyInterfacePtr & _hierarchy ) = 0;
        
    public:
        virtual void onHierarchyDeactivate() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HierarchyReceiverInterface> HierarchyReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class HierarchyInterface
        : public ServantInterface
    {
    public:
        virtual void setHierarchyReceiver( HierarchyReceiverInterface * _receiver ) const = 0;
        virtual HierarchyReceiverInterface * getHierarchyReceiver() const = 0;

    public:
        virtual HierarchyInterface * getParent() const = 0;
        virtual bool hasParent() const = 0;

    public:
        virtual void addChild( const HierarchyInterfacePtr & _hierarchy ) = 0;
        virtual void addChildFront( const HierarchyInterfacePtr & _hierarchy ) = 0;
        virtual bool addChildAfter( const HierarchyInterfacePtr & _hierarchy, const HierarchyInterfacePtr & _after ) = 0;
        virtual bool removeChild( const HierarchyInterfacePtr & _hierarchy ) = 0;

        typedef Lambda<void( const HierarchyInterfacePtr & _hierarchy )> LambdaRemoveChildren;
        virtual void removeChildren( const LambdaRemoveChildren & _lambda ) = 0;
        virtual bool removeFromParent() = 0;

        typedef Lambda<void( const HierarchyInterfacePtr & _hierarchy )> LambdaDestroyChildren;
        virtual void destroyChildren( const LambdaDestroyChildren & _lambda ) = 0;

        virtual uint32_t getChildrenRecursiveCount() const = 0;

        typedef Lambda<bool( const HierarchyInterfacePtr & )> LambdaFindChild;
        virtual bool hasChild( const LambdaFindChild & _lambda, bool _recursive ) const = 0;
        virtual HierarchyInterfacePtr findChild( const LambdaFindChild & _lambda, bool _recursion ) const = 0;

        virtual HierarchyInterfacePtr getSiblingPrev() const = 0;
        virtual HierarchyInterfacePtr getSiblingNext() const = 0;        
        virtual bool emptyChildren() const = 0;

    public:
        virtual uint32_t getLeafDeep() const = 0;

    public:
        typedef Lambda<void( const HierarchyInterfacePtr & )> LambdaNode;
        virtual void foreachChildren( const LambdaNode & _lambda ) const = 0;
        virtual void foreachChildrenUnslug( const LambdaNode & _lambda ) const = 0;
        virtual void foreachChildrenReverse( const LambdaNode & _lambda ) const = 0;

    public:
        typedef Lambda<bool( const HierarchyInterfacePtr & )> LambdaNodeBreak;
        virtual void foreachChildrenUnslugBreak( const LambdaNodeBreak & _lambda ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HierarchyInterface> HierarchyInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}