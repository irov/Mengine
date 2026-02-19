#pragma once

#include "Kernel/PluginBase.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "ResourcePrefetcherScriptEmbedding.h"
#endif

namespace Mengine
{
    class ResourcePrefetcherPlugin
        : public PluginBase
        , protected EXTEND_EMBEDDABLE( ResourcePrefetcherScriptEmbedding )
    {
        PLUGIN_DECLARE( "ResourcePrefetcher" );
        DECLARE_EMBEDDABLE();

    public:
        ResourcePrefetcherPlugin();
        ~ResourcePrefetcherPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}