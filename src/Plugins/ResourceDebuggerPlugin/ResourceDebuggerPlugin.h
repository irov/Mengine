#pragma once

#include "ResourceUselessCompileChecker.h"

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class ResourceDebuggerPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ResourceDebugger" )

    public:
        ResourceDebuggerPlugin();
        ~ResourceDebuggerPlugin() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        ResourceUselessCompileCheckerPtr m_resourceUselessCompileChecker;
    };
}