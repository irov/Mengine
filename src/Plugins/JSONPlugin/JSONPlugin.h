#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class JSONPlugin
        : public PluginBase
        , public Observable
    {
    public:
        PLUGIN_DECLARE( "JSONPlugin" )

    public:
        JSONPlugin();
        ~JSONPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}

