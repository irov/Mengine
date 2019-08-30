#pragma once

#include "cURLInterface.h"

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class cURLPlugin
        : public PluginBase
        , public Observable
    {
    public:
        PLUGIN_DECLARE( "cURL" );

    public:
        cURLPlugin();
        ~cURLPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}

