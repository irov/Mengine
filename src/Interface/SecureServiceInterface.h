#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/HashType.h"

namespace Mengine
{    
    class SecureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SecureService" );

    public:
        virtual HashType getSequreHash() const = 0;

    public:
        virtual void protectData( void * const _buffer, size_t _size ) const = 0;
        virtual void unprotectData( void * const _buffer, size_t _size ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SECURE_SERVICE()\
    ((Mengine::SecureServiceInterface*)SERVICE_GET(Mengine::SecureServiceInterface))
//////////////////////////////////////////////////////////////////////////