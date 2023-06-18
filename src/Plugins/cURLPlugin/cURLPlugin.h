#pragma once

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
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}