#pragma once

#include "Interface/CodecInterface.h"

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

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}

