#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class TexturepackerPlugin
        : public PluginBase
        , public Observable
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

