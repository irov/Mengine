#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Resource.h"

namespace Mengine
{
    class ResourceConvertPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ResourceConvert" )

    public:
        ResourceConvertPlugin();
        ~ResourceConvertPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void convertResource_( Resource * _resource );
    };
}