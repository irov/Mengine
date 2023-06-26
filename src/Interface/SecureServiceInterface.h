#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{    
    class SecureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SecureService" );

    public:
        virtual void protectData( uint32_t _complexity, const void * _in, size_t _size, void * const _out ) const = 0;
        virtual void unprotectData( uint32_t _complexity, const void * _in, size_t _size, void * const _out ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define SECURE_SERVICE()\
    ((Mengine::SecureServiceInterface*)SERVICE_GET(Mengine::SecureServiceInterface))
//////////////////////////////////////////////////////////////////////////