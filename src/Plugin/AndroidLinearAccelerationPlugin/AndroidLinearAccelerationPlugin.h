#pragma once

#include "Core/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidLinearAccelerationPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidLinearAcceleration" )

    public:
        AndroidLinearAccelerationPlugin();
        ~AndroidLinearAccelerationPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}