#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class SheenBidiPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "SheenBidi" );

    public:
        SheenBidiPlugin();
        ~SheenBidiPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
