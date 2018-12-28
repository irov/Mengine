#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class SoundCodecPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "SoundCodec" )

    public:
        SoundCodecPlugin();
        ~SoundCodecPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}