#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class VideoPlugin
        : public PluginBase
        , public Observable
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

