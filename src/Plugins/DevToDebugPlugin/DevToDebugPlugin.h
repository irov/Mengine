#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "DevToDebugScriptEmbedding.h"
#endif

namespace Mengine
{
    class DevToDebugPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( DevToDebugScriptEmbedding )
    {
        PLUGIN_DECLARE( "DevToDebug" );
        DECLARE_VISITABLE( PluginInterface );
        DECLARE_EMBEDDABLE();

    public:
        DevToDebugPlugin();
        ~DevToDebugPlugin() override;

    protected:
        bool _unimportantPlugin() const override;

    protected:
        const ServiceRequiredList & requiredServices() const override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}