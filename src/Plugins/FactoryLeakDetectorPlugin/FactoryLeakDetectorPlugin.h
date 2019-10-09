#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class FactoryLeakDetectorPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "FactoryLeakDetector" )

    public:
        FactoryLeakDetectorPlugin();
        ~FactoryLeakDetectorPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _systemPlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroy() override;
    };
}