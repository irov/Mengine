#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ChronometerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ChronometerService" );

    public:
        virtual void update() = 0;

    public:
        typedef Lambda<void( uint32_t, uint64_t )> LambdaChronometer;
        virtual uint32_t addChronometer( const LambdaChronometer & _lambda, const DocumentPtr & _doc ) = 0;
        virtual bool removeChronometer( uint32_t _id ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CHRONOMETER_SERVICE()\
    ((Mengine::ChronometerServiceInterface*)SERVICE_GET(Mengine::ChronometerServiceInterface))
//////////////////////////////////////////////////////////////////////////
