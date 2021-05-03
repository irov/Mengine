#pragma once

#include "Interface/ServantInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderOrderInterface
        : public ServantInterface
    {
    public:
        virtual void setIndex( int32_t _index ) = 0;
        virtual int32_t getIndex() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderOrderInterface> RenderOrderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}