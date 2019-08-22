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
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}