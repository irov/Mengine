#pragma once

#include "Interface/CodecInterface.h"
#include "Interface/DataInterface.h"

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