#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class OggVorbisPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "OggVorbis" )

    public:
        OggVorbisPlugin();
        ~OggVorbisPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}