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
        virtual void saveBase64( String * const _base64 ) const = 0;
        virtual void loadBase64( const String & _base64 ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SecureValueInterface> SecureValueInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}