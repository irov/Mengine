#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t INVALID_UPDATABLE_ID = ~0U;
    //////////////////////////////////////////////////////////////////////////
    struct UpdateContext
    {
        uint32_t revision;
        float current;
        float time;
    };
    //////////////////////////////////////////////////////////////////////////
    class UpdationInterface
        : public Mixin
    {
    public:
        virtual void update( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UpdationInterface> UpdationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////        
    class UpdateServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "UpdateService" )

    public:
        virtual uint32_t createUpdatater( uint32_t _mode, uint32_t _leaf, const UpdationInterfacePtr & _updation ) = 0;
        virtual void removeUpdatater( uint32_t _id ) = 0;

    public:
        virtual void update( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define UPDATE_SERVICE()\
    ((Mengine::UpdateServiceInterface *)SERVICE_GET(Mengine::UpdateServiceInterface))
    //////////////////////////////////////////////////////////////////////////
}