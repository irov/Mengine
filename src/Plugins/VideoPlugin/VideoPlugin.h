#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class VideoPlugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "Video" )

    public:
        VideoPlugin();
        ~VideoPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}

