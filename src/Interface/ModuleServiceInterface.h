#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/ModuleFactoryInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"
#include "Kernel/Params.h"

namespace Mengine
{
    class ModuleServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ModuleService" )

    public:
        virtual void finalizeModules() = 0;

    public:
        virtual bool registerModule( const ConstString & _moduleName, const ModuleFactoryInterfacePtr & _factory ) = 0;
        virtual void unregisterModule( const ConstString & _moduleName ) = 0;

    public:
        virtual bool hasModule( const ConstString & _moduleName ) const = 0;
        virtual bool runModule( const ConstString & _moduleName, const Char * _doc ) = 0;
        virtual bool stopModule( const ConstString & _moduleName ) = 0;

    public:
        virtual void update( bool _focus ) = 0;

    public:
        virtual void tick( const UpdateContext * _context ) = 0;
        virtual void render( const RenderContext * _context ) = 0;

    public:
        virtual void message( const ConstString & _moduleName, const ConstString & _messageName, const MapWParams & _params ) = 0;
        virtual void messageAll( const ConstString & _messageName, const MapWParams & _params ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MODULE_SERVICE()\
	((Mengine::ModuleServiceInterface *)SERVICE_GET(Mengine::ModuleServiceInterface))
//////////////////////////////////////////////////////////////////////////