#pragma once

#include "Kernel/ModuleBase.h"

namespace Mengine
{
    class ModuleCameraDebugGizmo
        : public ModuleBase
    {
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

        uint32_t m_mouseWheelEventHandle;
        uint32_t m_mouseMoveEventHandle;
        uint32_t m_keyEventHandle;
    };
}