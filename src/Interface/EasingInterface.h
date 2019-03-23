#pragma once

#include "Kernel/Factorable.h"

namespace Mengine
{
    class EasingInterface
        : public Factorable
    {
    public:
        virtual float easing( float _t ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<EasingInterface> EasingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}