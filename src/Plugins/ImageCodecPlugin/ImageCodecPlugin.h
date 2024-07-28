#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class ImageCodecPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ImageCodec" )

    public:
        ImageCodecPlugin();
        ~ImageCodecPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}