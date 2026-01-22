#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class CameraDebugGizmoPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "CameraDebugGizmo" );

    public:
        CameraDebugGizmoPlugin();
        ~CameraDebugGizmoPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}