#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Document.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"
#include "Config/Timestamp.h"

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
        typedef Lambda<void( UniqueId _id, Timestamp _milliseconds )> LambdaChronometer;

        virtual UniqueId addChronometer( const LambdaChronometer & _lambda, const DocumentPtr & _doc ) = 0;
        virtual bool removeChronometer( UniqueId _id ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CHRONOMETER_SERVICE()\
    ((Mengine::ChronometerServiceInterface*)SERVICE_GET(Mengine::ChronometerServiceInterface))
//////////////////////////////////////////////////////////////////////////
