#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/UpdationInterface.h"

#include "Kernel/UpdateMode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t INVALID_UPDATABLE_ID = ~0U;
    //////////////////////////////////////////////////////////////////////////        
    class UpdateServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "UpdateService" )

    public:
        virtual void placeUpdatater( UpdationInterface * _updation ) = 0;
        virtual void removeUpdatater( UpdationInterface * _updation ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define UPDATE_SERVICE()\
    ((Mengine::UpdateServiceInterface *)SERVICE_GET(Mengine::UpdateServiceInterface))
//////////////////////////////////////////////////////////////////////////
