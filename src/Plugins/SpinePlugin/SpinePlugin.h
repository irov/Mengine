#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class SpinePlugin
        : public PluginBase
        , public Observable
    {
        PLUGIN_DECLARE( "Spine" )

    public:
        SpinePlugin();
        ~SpinePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}