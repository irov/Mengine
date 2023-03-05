#pragma once

#include "Kernel/ModuleBase.h"

namespace Mengine
{
    class ModuleCameraDebugGizmo
        : public ModuleBase
    {
        DECLARE_FACTORABLE( ModuleCameraDebugGizmo );

    public:
        ModuleCameraDebugGizmo();
        ~ModuleCameraDebugGizmo() override;

    public:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        bool _handleKeyEvent( const InputKeyEvent & _event ) override;
        bool _handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool _handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    protected:
        float m_scaleStepMin;
        float m_scaleStepStep;
        float m_scaleStepMax;
    };
}