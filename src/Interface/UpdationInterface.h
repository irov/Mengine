#pragma once

#include "Interface/Interface.h"

#include "Kernel/UpdateContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EUpdateMode
    {
        EUM_NODE_BASE,
        EUM_NODE_AFFECTOR,
        EUM_SERVICE,
    };
    //////////////////////////////////////////////////////////////////////////
    class UpdationInterface
        : public Interface
    {
    public:
        virtual void activate( EUpdateMode _mode, uint32_t _deep ) = 0;
        virtual void deactivate() = 0;
        virtual void replace( uint32_t _deep ) = 0;

    public:
        virtual void update( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UpdationInterface> UpdationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////        
}