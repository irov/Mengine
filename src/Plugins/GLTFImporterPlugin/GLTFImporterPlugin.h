#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class GLTFImporterPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "GLTFImporter" );

    public:
        GLTFImporterPlugin();
        ~GLTFImporterPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
