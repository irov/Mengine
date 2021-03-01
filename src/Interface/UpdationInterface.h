#pragma once

#include "Interface/Interface.h"

#include "Kernel/UpdateContext.h"

#include "Kernel/UpdateMode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Updatable;
    //////////////////////////////////////////////////////////////////////////
    class UpdationInterface
        : public Interface
    {
    public:
        virtual Updatable * getUpdatable() = 0;

    public:
        virtual bool activate( EUpdateMode _mode, uint32_t _deep ) = 0;
        virtual void deactivate() = 0;
        virtual void replace( uint32_t _deep ) = 0;

    public:
        virtual void update( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UpdationInterface> UpdationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////        
}