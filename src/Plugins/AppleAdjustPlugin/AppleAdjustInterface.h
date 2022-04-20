#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleAdjustServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AppleAdjustService" )

    public:

        virtual void eventTraking( const ConstString& token ) const = 0;
        virtual void revenueTracking( const ConstString& token, double amount, const ConstString& currency ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define APPLE_ADJUST_SERVICE()\
    ((AppleAdjustServiceInterface *)SERVICE_GET(Mengine::AppleAdjustServiceInterface))
//////////////////////////////////////////////////////////////////////////
