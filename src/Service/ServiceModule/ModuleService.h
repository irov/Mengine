#pragma once

#include "Interface/ModuleInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Map.h"
#include "Config/Vector.h"

namespace Mengine
{
    class ModuleService
        : public ServiceBase<ModuleServiceInterface>
    {
    public:
        ModuleService();
        ~ModuleService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool registerModule( const ConstString & _name, const ModuleFactoryInterfacePtr & _module ) override;
        void unregisterModule( const ConstString & _name ) override;

    public:
        bool runModule( const ConstString & _name ) override;
        void stopModule( const ConstString & _name ) override;

    public:
        void update( bool _focus ) override;

    public:
        void tick( const UpdateContext * _context ) override;
        void render( const RenderContext * _state ) override;

    public:
        void message( const ConstString & _moduleName, const ConstString & _messageName, const MapWParams & _params ) override;
        void messageAll( const ConstString & _messageName, const MapWParams & _params ) override;

    protected:
        const ModuleInterfacePtr & findModule( const ConstString & _moduleName ) const;

    protected:
        typedef Map<ConstString, ModuleFactoryInterfacePtr> MapModuleFactory;
        MapModuleFactory m_moduleFactory;

        typedef Vector<ModuleInterfacePtr> VectorModules;
        VectorModules m_modules;
    };
}