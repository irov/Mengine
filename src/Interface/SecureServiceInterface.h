#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/HashType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SecureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SequreService" );

    public:
        virtual HashType getSequreHash() const = 0;

    public:
        virtual void protectData( void * _buffer, size_t _size ) const = 0;
        virtual void unprotectData( void * _buffer, size_t _size ) const = 0;
    };
}