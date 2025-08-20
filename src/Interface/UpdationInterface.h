#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/UpdateContext.h"
#include "Kernel/UpdateMode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Updatable;
    //////////////////////////////////////////////////////////////////////////
    class UpdationInterface
        : public Mixin
    {
    public:
        virtual Updatable * getUpdatable() = 0;

    public:
        virtual void activate( EUpdateMode _mode, uint32_t _deep ) = 0;
        virtual void deactivate() = 0;
        virtual void replace( uint32_t _deep ) = 0;

    public:
        virtual EUpdateMode getUpdationMode() const = 0;
        virtual uint32_t getUpdationDeep() const = 0;

    public:
        virtual void update( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UpdationInterface> UpdationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////        
}