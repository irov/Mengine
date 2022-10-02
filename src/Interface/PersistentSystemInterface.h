#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    class PersistentSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PersistentSystem" )

    public:
        virtual bool getPersistentArguments( Char * _arguments, size_t _capacity ) const = 0;
        virtual void setPersistentArguments( const Char * _arguments ) = 0;
        virtual void removePersistentArguments() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PERSISTENT_SYSTEM()\
    ((Mengine::PersistentSystemInterface *)SERVICE_GET(Mengine::PersistentSystemInterface))
//////////////////////////////////////////////////////////////////////////