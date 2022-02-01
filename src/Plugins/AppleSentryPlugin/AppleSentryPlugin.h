#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/ErrorLevel.h"
#include "Kernel/AssertionLevel.h"

namespace Mengine
{    
    class AppleSentryPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleSentry" )

    public:
        AppleSentryPlugin();
        ~AppleSentryPlugin() override;

    protected:
        bool _unimportantPlugin() const override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void notifyCreateApplication_();
    };
}