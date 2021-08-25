#pragma once

#include "Interface/DebuggerBoundingBoxInterface.h"

#include "Kernel/NodeCast.h"

namespace Mengine
{
    template<class T>
    class BaseDebuggerBoundingBox
        : public DebuggerBoundingBoxInterface
    {
    protected:
        bool getBoundingBox( const NodePtr & _node, mt::box2f * const _bb ) const override
        {
            T t = Helper::staticNodeCast<T>( _node );

            bool successful = this->_getBoundingBox( t, _bb );

            return successful;
        }

    protected:
        virtual bool _getBoundingBox( const T & _node, mt::box2f * const _bb ) const = 0;
    };
}