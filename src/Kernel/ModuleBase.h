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
        bool isAvailableModule() const override;
        bool isInitializeModule() const override;

    public:
        bool initializeModule() override;
        void finalizeModule() override;

    protected:
        virtual bool _availableModule() const;

    protected:
        virtual bool _initializeModule();
        virtual void _finalizeModule();

    public:
        void beginUpdate( bool _focus ) override;
        void endUpdate() override;

    protected:
        virtual void _beginUpdate( bool _focus );
        virtual void _endUpdate();

    public:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        virtual void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context );

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    protected:
        virtual bool _handleKeyEvent( const InputKeyEvent & _event );
        virtual bool _handleTextEvent( const InputTextEvent & _event );

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    protected:
        virtual bool _handleMouseButtonEvent( const InputMouseButtonEvent & _event );
        virtual bool _handleMouseButtonEventBegin( const InputMouseButtonEvent & _event );
        virtual bool _handleMouseButtonEventEnd( const InputMouseButtonEvent & _event );
        virtual bool _handleMouseMove( const InputMouseMoveEvent & _event );
        virtual bool _handleMouseWheel( const InputMouseWheelEvent & _event );

    public:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    protected:
        virtual bool _handleMouseEnter( const InputMouseEnterEvent & _event );
        virtual void _handleMouseLeave( const InputMouseLeaveEvent & _event );

    protected:
        ConstString m_name;

        bool m_availableModule;
        bool m_initializeModule;
    };
}