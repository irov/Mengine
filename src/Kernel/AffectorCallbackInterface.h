#pragma once

#include "Kernel/Mixin.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AffectorCallbackInterface
        : public Mixin
    {
    public:
        virtual void onAffectorEnd( UniqueId _id, bool _isEnd ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AffectorCallbackInterface> AffectorCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}
