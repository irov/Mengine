#pragma once

#include "RenderDocInterface.h"

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
        RENDERDOC_API_1_1_2 * m_renderDocAPI;
    };
}
