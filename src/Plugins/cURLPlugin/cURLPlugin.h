#pragma once

#include "cURLInterface.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class cURLPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "cURL" );

    public:
        cURLPlugin();
        ~cURLPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}