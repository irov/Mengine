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
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;
    };
}