#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class TexturepackerPlugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "Texturepacker" )

    public:
		TexturepackerPlugin();
        ~TexturepackerPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}

