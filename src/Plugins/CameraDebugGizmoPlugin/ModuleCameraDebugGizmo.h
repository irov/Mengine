#pragma once

#include "Kernel/ModuleBase.h"

#include "Config/UniqueId.h"

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
        float m_scaleStepMin;
        float m_scaleStepStep;
        float m_scaleStepMax;

        UniqueId m_mouseWheelEventHandle;
        UniqueId m_mouseMoveEventHandle;
        UniqueId m_keyEventHandle;
    };
}