#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Node.h"

#include "math/box2.h"

namespace Mengine
{
    class NodeDebuggerBoundingBoxInterface
        : public Mixin
    {
    public:
        virtual bool getBoundingBox( const NodePtr & _node, mt::box2f * _bb ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NodeDebuggerBoundingBoxInterface> NodeDebuggerBoundingBoxInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class BaseDebuggerBoundingBox
        : public NodeDebuggerBoundingBoxInterface
    {
    protected:
        bool getBoundingBox( const NodePtr & _node, mt::box2f * _bb ) override
        {
            IntrusivePtr<T> t = stdex::intrusive_static_cast<IntrusivePtr<T>>(_node);

            bool successful = this->_getBoundingBox( t, _bb );

            return successful;
        }

    protected:
        virtual bool _getBoundingBox( const IntrusivePtr<T> & _node, mt::box2f * _bb ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
