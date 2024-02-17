#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class RenderDocPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "RenderDoc" )

    public:
        RenderDocPlugin();
        ~RenderDocPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}