#pragma once

#include "Kernel/ModuleBase.h"

namespace Mengine
{
    class ImGUIModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( ImGUIModule );

    public:
        ImGUIModule();
        ~ImGUIModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void notifyCreateRenderWindow_();
        void notifyRenderDeviceLostPrepare_();
        void notifyRenderDeviceLostRestore_();

    protected:
        uint32_t m_handlerId;
    };
}