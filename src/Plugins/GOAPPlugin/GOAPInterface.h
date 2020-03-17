#pragma once

#include "Interface/ServiceInterface.h"

#include "EngineCook.h"

namespace Mengine
{
    class GOAPServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "GOAPService" );

    public:
        virtual GOAP::SourceInterfacePtr makeSource() = 0;
        virtual GOAP::ChainInterfacePtr makeChain( const GOAP::SourceInterfacePtr & _source, const Char * _file, uint32_t _line ) = 0;

    public:
        virtual GOAP::SemaphoreInterfacePtr makeSemaphore( int32_t _value ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define GOAP_SERVICE()\
    ((Mengine::GOAPServiceInterface*)SERVICE_GET(Mengine::GOAPServiceInterface))
//////////////////////////////////////////////////////////////////////////