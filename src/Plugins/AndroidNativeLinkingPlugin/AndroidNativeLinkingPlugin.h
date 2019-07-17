#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AndroidNativeLinkingPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeLinking" )

    public:
        AndroidNativeLinkingPlugin();
        ~AndroidNativeLinkingPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
