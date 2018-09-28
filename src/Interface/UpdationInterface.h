#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
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
        virtual void activate( uint32_t _mode, uint32_t _deep ) = 0;
        virtual void deactivate() = 0;
        virtual void replace( uint32_t _deep ) = 0;

    public:
        virtual void update( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UpdationInterface> UpdationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////        
}