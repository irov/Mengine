#pragma once

#include "Interface/CodecInterface.h"

#include "Kernel/PluginBase.h"

#include "Config/Vector.h"

namespace Mengine
{
    class OzzAnimationPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "OzzAnimation" )

    public:
        OzzAnimationPlugin();
        ~OzzAnimationPlugin() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}