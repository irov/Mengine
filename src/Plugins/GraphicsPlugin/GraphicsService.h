#pragma once

#include "GraphicsInterface.h"

#include "Kernel/ServiceBase.h"

#include "graphics/graphics.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GraphicsService
        : public ServiceBase<GraphicsServiceInterface>
    {
        DECLARE_FACTORABLE( GraphicsService );

    public:
        GraphicsService();
        ~GraphicsService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        gp_graphics_t * getGraphics() const override;
        const RenderMaterialStage * getGradientMaterialStage( bool _radial ) override;
        uint64_t getGradientTextureGeneration() const override;

    protected:
        void notifyRenderDeviceLostRestore_();

    protected:
        gp_graphics_t * m_graphics;
        const RenderMaterialStage * m_linearGradientStage;
        const RenderMaterialStage * m_radialGradientStage;
        uint64_t m_gradientTextureGeneration;
    };
    //////////////////////////////////////////////////////////////////////////
}
