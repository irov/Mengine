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
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        gp_graphics_t * getGraphics() const override;

    protected:
        gp_graphics_t * m_graphics;
    };
    //////////////////////////////////////////////////////////////////////////
}
