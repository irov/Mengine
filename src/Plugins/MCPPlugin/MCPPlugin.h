#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "MCP" );

    public:
        MCPPlugin();
        ~MCPPlugin() override;

    protected:
        bool _unimportantPlugin() const override;
        const ServiceRequiredList & requiredServices() const override;
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
    //////////////////////////////////////////////////////////////////////////
}
