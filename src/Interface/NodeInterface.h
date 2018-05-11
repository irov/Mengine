#pragma once

#include "Interface/ServiceInterface.h"

#include "Core/ConstString.h"
#include "Core/IntrusivePtr.h"
#include "Core/Pointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Node> NodePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef PointerT<NodePtr> PointerNode;
    //////////////////////////////////////////////////////////////////////////
    class NodeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("NodeService")

    public:
        virtual PointerNode createNode( const ConstString& _type ) = 0;

    public:
        virtual void addHomeless( const NodePtr & _homeless ) = 0;
		virtual bool isHomeless( const NodePtr & _node ) const = 0;
        virtual void clearHomeless() = 0;
    };

#   define NODE_SERVICE()\
    ((Mengine::NodeServiceInterface *)SERVICE_GET(Mengine::NodeServiceInterface))
}