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
        bool hasModule( const ConstString & _name ) const override;
        bool runModule( const ConstString & _name, const Char * _doc ) override;
        bool stopModule( const ConstString & _name ) override;
        bool isRunModule( const ConstString & _name ) const override;

    public:
        void update( bool _focus ) override;

    public:
        void tick( const UpdateContext * _context ) override;
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    public:
        void message( const ConstString & _name, const ConstString & _messageName, const MapWParams & _params ) override;
        void messageAll( const ConstString & _messageName, const MapWParams & _params ) override;

    protected:
        const ModuleInterfacePtr & findModule_( const ConstString & _name ) const;
        ModuleInterfacePtr popModule_( const ConstString & _name );

    protected:
        typedef Vector<ModuleInterfacePtr> VectorModules;
        VectorModules m_modules;
    };
}