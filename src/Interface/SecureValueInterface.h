#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SecureValueInterface
        : public ServantInterface
    {
    public:
        virtual void saveHexadecimal( String * const _hexadecimal ) const = 0;
        virtual bool loadHexadecimal( const String & _hexadecimal ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SecureValueInterface> SecureValueInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}