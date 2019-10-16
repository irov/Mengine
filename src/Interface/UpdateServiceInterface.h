#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/UpdationInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t INVALID_UPDATABLE_ID = ~0U;
    //////////////////////////////////////////////////////////////////////////        
    class UpdateServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "UpdateService" )

    public:
        virtual uint32_t createUpdatater( uint32_t _mode, uint32_t _deep, const UpdationInterfacePtr & _updation ) = 0;
        virtual void replaceUpdatater( uint32_t _id, uint32_t _deep ) = 0;
        virtual void removeUpdatater( uint32_t _id ) = 0;

    public:
        virtual void update( const UpdateContext * _context ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define UPDATE_SERVICE()\
    ((Mengine::UpdateServiceInterface *)SERVICE_GET(Mengine::UpdateServiceInterface))
//////////////////////////////////////////////////////////////////////////
