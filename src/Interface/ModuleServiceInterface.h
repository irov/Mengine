#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/RenderPipelineInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{    
    class ModuleServiceInterface
        : public ServiceInterface
        , public InputHandlerInterface
    {
        SERVICE_DECLARE( "ModuleService" )

    public:
        virtual bool hasModule( const ConstString & _name ) const = 0;
        virtual bool runModule( const ConstString & _name, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool stopModule( const ConstString & _name ) = 0;
        virtual bool isRunModule( const ConstString & _name ) const = 0;

    public:
        virtual const ModuleInterfacePtr & getModule( const ConstString & _name ) const = 0;

    public:
        typedef Lambda<void()> LambdaWaitModule;
        virtual void waitModule( const ConstString & _name, const LambdaWaitModule & _lambda ) = 0;

        typedef Lambda<void()> LambdaLeaveModule;
        virtual void leaveModule( const ConstString & _name, const LambdaLeaveModule & _lambda ) = 0;

    public:
        virtual void beginUpdate( bool _focus ) = 0;
        virtual void endUpdate() = 0;

    public:
        virtual void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MODULE_SERVICE()\
    ((Mengine::ModuleServiceInterface *)SERVICE_GET(Mengine::ModuleServiceInterface))
//////////////////////////////////////////////////////////////////////////