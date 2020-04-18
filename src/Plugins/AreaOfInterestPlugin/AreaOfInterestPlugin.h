#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AreaOfInterestPlugin
        : public PluginBase
        , public Observable
    {
        PLUGIN_DECLARE( "AreaOfInterest" )

    public:
        AreaOfInterestPlugin();
        ~AreaOfInterestPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}