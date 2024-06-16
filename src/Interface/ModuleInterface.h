#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/InputHandlerInterface.h"

#include "Kernel/UpdateContext.h"
#include "Kernel/RenderContext.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ModuleInterface
        : public ServantInterface
        , public InputHandlerInterface
    {
    public:
        virtual void setName( const ConstString & _name ) = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual bool isAvailableModule() const = 0;
        virtual bool isInitializeModule() const = 0;

    public:
        virtual bool initializeModule() = 0;
        virtual void finalizeModule() = 0;

    public:
        virtual void beginUpdate( bool _focus ) = 0;
        virtual void endUpdate() = 0;

    public:
        virtual void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ModuleInterface> ModuleInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}