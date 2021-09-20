#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class ResourceValidatePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "ResourceValidate" )

    public:
        ResourceValidatePlugin();
        ~ResourceValidatePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}