#pragma once

#include "Interface/ThreadInterface.h"

#include "Core/PluginBase.h"

namespace Mengine 
{
    class AndroidNativeUnityPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeUnity" )

    public:
        AndroidNativeUnityPlugin();
        ~AndroidNativeUnityPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}
