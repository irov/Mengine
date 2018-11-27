#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceConvertPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ResourceConvert" )

    public:
        ResourceConvertPlugin();
        ~ResourceConvertPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;
    };
}