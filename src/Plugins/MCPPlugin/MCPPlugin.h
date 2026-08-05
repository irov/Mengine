#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Params.h"
#include "Kernel/String.h"

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

#if defined(MENGINE_PLATFORM_ANDROID)
    protected:
        void notifyApplicationIntent_( const String & _action, const String & _data, const String & _type, uint32_t _flags, const Params & _extras );
#endif
    };
    //////////////////////////////////////////////////////////////////////////
}
