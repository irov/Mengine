#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AndroidNativeFacebookPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeFacebook" )

    public:
        AndroidNativeFacebookPlugin();
        ~AndroidNativeFacebookPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}
