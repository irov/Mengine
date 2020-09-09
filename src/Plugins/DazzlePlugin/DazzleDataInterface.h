#pragma once

#include "Interface/DataInterface.h"

#include "dazzle/dazzle.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DazzleDataInterface
        : public DataInterface
    {
    public:
        virtual const dz_service_t * getDazzleService() const = 0;
        virtual const dz_effect_t * getDazzleEffect() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DazzleDataInterface> DazzleDataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}