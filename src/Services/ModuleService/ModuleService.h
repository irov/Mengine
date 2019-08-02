#pragma once

#include "Interface/ModuleServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

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
        void finalizeModules() override;

    public:
        bool registerModule( const ConstString & _moduleName, const ModuleFactoryInterfacePtr & _module ) override;
        void unregisterModule( const ConstString & _moduleName ) override;

    public:
        bool runModule( const ConstString & _moduleName, const Char * _doc ) override;
        bool stopModule( const ConstString & _moduleName ) override;

    public:
        void update( bool _focus ) override;

    public:
        void tick( const UpdateContext * _context ) override;
        void render( const RenderContext * _context ) override;

    public:
        void message( const ConstString & _moduleName, const ConstString & _messageName, const MapWParams & _params ) override;
        void messageAll( const ConstString & _messageName, const MapWParams & _params ) override;

    protected:
        const ModuleInterfacePtr & findModule_( const ConstString & _moduleName ) const;
        ModuleInterfacePtr popModule_( const ConstString & _moduleName );

    protected:
        typedef Hashtable<ConstString, ModuleFactoryInterfacePtr> HashtableModuleFactory;
        HashtableModuleFactory m_moduleFactory;

        typedef Vector<ModuleInterfacePtr> VectorModules;
        VectorModules m_modules;
    };
}