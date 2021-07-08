#pragma once

#include "Kernel/AffectorType.h"
#include "Kernel/Updatable.h"
#include "Kernel/Factorable.h"
#include "Kernel/UpdateContext.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AffectorCallbackInterface
        : public Factorable
    {
    public:
        virtual void onAffectorEnd( UniqueId _id, bool _isEnd ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AffectorCallbackInterface> AffectorCallbackInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}
