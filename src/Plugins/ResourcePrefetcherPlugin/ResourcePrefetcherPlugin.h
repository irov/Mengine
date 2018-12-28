#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourcePrefetcherPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ResourcePrefetcher" )

    public:
        ResourcePrefetcherPlugin();
        ~ResourcePrefetcherPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroy() override;
    };
}