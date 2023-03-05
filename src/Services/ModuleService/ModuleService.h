#pragma once

#include "Interface/ModuleServiceInterface.h"
#include "Interface/TimepipeInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Vector.h"

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
        bool runModule( const ConstString & _name, const DocumentPtr & _doc ) override;
        bool stopModule( const ConstString & _name ) override;
        bool isRunModule( const ConstString & _name ) const override;

    public:
        const ModuleInterfacePtr & getModule( const ConstString & _name ) const override;

    public:
        void waitModule( const ConstString & _name, const LambdaWaitModule & _lambda ) override;
        void leaveModule( const ConstString & _name, const LambdaLeaveModule & _lambda ) override;

    public:
        void beginUpdate( bool _focus ) override;
        void endUpdate() override;

    public:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    public:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    protected:
        const ModuleInterfacePtr & findModule_( const ConstString & _name ) const;
        ModuleInterfacePtr popModule_( const ConstString & _name );

    protected:
        typedef Vector<ModuleInterfacePtr> VectorModules;
        VectorModules m_modules;

        struct WaitModuleDesc
        {
            ConstString name;
            LambdaWaitModule lambda;
        };

        typedef Vector<WaitModuleDesc> VectorModuleWaits;
        VectorModuleWaits m_waits;

        struct LeaveModuleDesc
        {
            ConstString name;
            LambdaLeaveModule lambda;
        };

        typedef Vector<LeaveModuleDesc> VectorModuleLeaves;
        VectorModuleLeaves m_leaves;
    };
}