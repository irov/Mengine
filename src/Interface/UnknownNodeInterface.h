#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Node.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownNodeInterface
        : public UnknownInterface
    {
    public:
        virtual Node * getUnknownNode() = 0;
        virtual const Node * getUnknownNode() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownNodeInterface> UnknownNodeInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_NODE_UNKNOWABLE()\
public:\
    Mengine::Node * getUnknownNode() override{ return this; }\
    const Mengine::Node * getUnknownNode() const override{ return this; }\
protected: