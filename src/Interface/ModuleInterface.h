#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/UpdationInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Params.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderContext;
    //////////////////////////////////////////////////////////////////////////
    class ModuleInterface
        : public ServantInterface
    {
    public:
        virtual bool avaliable() = 0;

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
    class ModuleFactoryInterface
        : public ServantInterface
    {
    public:
        virtual ModuleInterfacePtr createModule( const ConstString & _name ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ModuleFactoryInterface> ModuleFactoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class ModuleServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ModuleService" )

    public:
        virtual bool registerModule( const ConstString & _name, const ModuleFactoryInterfacePtr & _factory ) = 0;
        virtual void unregisterModule( const ConstString & _name ) = 0;

    public:
        virtual bool runModule( const ConstString & _name ) = 0;
        virtual void stopModule( const ConstString & _name ) = 0;

    public:
        virtual void update( bool _focus ) = 0;

    public:
        virtual void tick( const UpdateContext * _context ) = 0;
        virtual void render( const RenderContext * _state ) = 0;

    public:
        virtual void message( const ConstString & _moduleName, const ConstString & _messageName, const MapWParams & _params ) = 0;
        virtual void messageAll( const ConstString & _messageName, const MapWParams & _params ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define MODULE_SERVICE()\
	((Mengine::ModuleServiceInterface *)SERVICE_GET(Mengine::ModuleServiceInterface))
}