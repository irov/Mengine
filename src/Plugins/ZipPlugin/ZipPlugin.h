#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class ZipPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Zip" );

    public:
        ZipPlugin();
        ~ZipPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}