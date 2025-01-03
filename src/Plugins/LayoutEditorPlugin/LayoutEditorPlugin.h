#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class LayoutEditorPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "LayoutEditor" );

    public:
        LayoutEditorPlugin();
        ~LayoutEditorPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}