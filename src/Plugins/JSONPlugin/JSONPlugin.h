#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "JSONScriptEmbedding.h"
#endif

namespace Mengine
{
    class JSONPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( JSONScriptEmbedding )
    {
    public:
        PLUGIN_DECLARE( "JSON" );
        DECLARE_EMBEDDABLE();

    public:
        JSONPlugin();
        ~JSONPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}

