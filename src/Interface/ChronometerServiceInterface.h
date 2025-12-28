#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/DocumentInterface.h"
#include "Interface/ChronometerInterface.h"

#include "Kernel/LambdaChronometer.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"
#include "Config/Timestamp.h"

namespace Mengine
{
    class ChronometerServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ChronometerService" );

    public:
        virtual ChronometerInterfacePtr addChronometer( const LambdaChronometer & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool removeChronometer( const ChronometerInterfacePtr & _chronometer ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CHRONOMETER_SERVICE()\
    ((Mengine::ChronometerServiceInterface *)SERVICE_GET(Mengine::ChronometerServiceInterface))
//////////////////////////////////////////////////////////////////////////
