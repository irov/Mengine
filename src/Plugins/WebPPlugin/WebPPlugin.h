#pragma once

#include "Interface/DataInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class WebPPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "WebP" );

    public:
        WebPPlugin();
        ~WebPPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}