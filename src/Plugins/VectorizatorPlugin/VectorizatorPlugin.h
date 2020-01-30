#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class VectorizatorPlugin
        : public PluginBase
        , public Observable
    {
        PLUGIN_DECLARE( "Vectorizator" )

    public:
        VectorizatorPlugin();
        ~VectorizatorPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}