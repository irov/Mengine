#pragma once

#include "Interface/ModuleInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class ModuleBase
        : public ModuleInterface
        , public Observable
    {
    public:
        ModuleBase();
        ~ModuleBase() override;

    public:
        void setName( const ConstString & _name ) override;
        const ConstString & getName() const override;

    public:
        bool initializeModule() override;
        void finalizeModule() override;

    public:
        bool isAvailableModule() const override;
        bool isInitializeModule() const override;

    protected:
        virtual bool _availableModule() const;

    protected:
        virtual bool _initializeModule();
        virtual void _finalizeModule();

    public:
        void update( bool _focus ) override;

    protected:
        virtual void _update( bool _focus );

    public:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        virtual void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context );

    public:
        void message( const ConstString & _messageName, const MapWParams & _params ) override;
        void messageAll( const ConstString & _messageName, const MapWParams & _params ) override;

    protected:
        virtual void _message( const ConstString & _messageName, const MapWParams & _params );
        virtual void _messageAll( const ConstString & _messageName, const MapWParams & _params );

    protected:
        ConstString m_name;

        bool m_availableModule;
        bool m_initializeModule;
    };
}