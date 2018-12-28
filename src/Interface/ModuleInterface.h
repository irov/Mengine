#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/UpdateContext.h"
#include "Kernel/RenderContext.h"
#include "Kernel/ConstString.h"
#include "Kernel/Params.h"

namespace Mengine
{
    class ModuleInterface
        : public Mixin
    {
    public:
        virtual bool available() = 0;

    public:
        virtual void setName( const ConstString & _name ) = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void update( bool _focus ) = 0;

    public:
        virtual void tick( const UpdateContext * _context ) = 0;
        virtual void render( const RenderContext * _state ) = 0;

    public:
        virtual void message( const ConstString & _messageName, const MapWParams & _params ) = 0;
        virtual void messageAll( const ConstString & _messageName, const MapWParams & _params ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ModuleInterface> ModuleInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}