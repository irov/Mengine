#pragma once

#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeKernelModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( AndroidNativeKernelModule );

    public:
        AndroidNativeKernelModule();
        ~AndroidNativeKernelModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void _update( bool _focus ) override;

    public:
        String getAndroidId() const;
    };
}
