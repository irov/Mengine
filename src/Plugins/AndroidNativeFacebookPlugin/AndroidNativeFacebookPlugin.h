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
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
