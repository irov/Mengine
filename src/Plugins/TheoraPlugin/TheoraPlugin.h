#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class TheoraPlugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "Theora" );

    public:
        TheoraPlugin();
        ~TheoraPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}

