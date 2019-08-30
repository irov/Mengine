#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourcePrefetcherPlugin
        : public PluginBase
        , public Observable
    {
        PLUGIN_DECLARE( "ResourcePrefetcher" )

    public:
        ResourcePrefetcherPlugin();
        ~ResourcePrefetcherPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}