#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{    
    class SecureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SecureService" );

    public:
        virtual uint64_t getSecureHash() const = 0;

    public:
        virtual void protectData( void * const _buffer, size_t _size ) const = 0;
        virtual void unprotectData( void * const _buffer, size_t _size ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SECURE_SERVICE()\
    ((Mengine::SecureServiceInterface*)SERVICE_GET(Mengine::SecureServiceInterface))
//////////////////////////////////////////////////////////////////////////