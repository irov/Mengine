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
        virtual uint32_t createUpdatater( EUpdateMode _mode, uint32_t _deep, const UpdationInterfacePtr & _updation ) = 0;
        virtual void replaceUpdatater( uint32_t _id, uint32_t _deep ) = 0;
        virtual void removeUpdatater( uint32_t _id ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define UPDATE_SERVICE()\
    ((Mengine::UpdateServiceInterface *)SERVICE_GET(Mengine::UpdateServiceInterface))
//////////////////////////////////////////////////////////////////////////
