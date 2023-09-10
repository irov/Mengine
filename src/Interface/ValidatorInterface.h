#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ValidatorInterface
        : public ServantInterface
    {
    public:
        virtual bool validate( const FactorablePtr & _factorable ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ValidatorInterface> ValidatorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
