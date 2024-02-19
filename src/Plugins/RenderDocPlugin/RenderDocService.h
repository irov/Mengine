#pragma once

#include "Interface/RenderPipelineInterface.h"

#include "RenderDocInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ServiceBase.h"

#include "renderdoc/api/app/renderdoc_app.h"

namespace Mengine
{
    class RenderDocService
        : public ServiceBase<RenderDocServiceInterface>
    {
    public:
        RenderDocService();
        ~RenderDocService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void notifyCreateRenderWindow_();
        void notifyRenderSceneBegin_( const RenderPipelineInterfacePtr & _pipeline );
        void notifyRenderSceneEnd_( const RenderPipelineInterfacePtr & _pipeline );

    protected:
        HMODULE m_renderDocModule;

        RENDERDOC_API_1_6_0 * m_renderDocAPI;
    };
}
