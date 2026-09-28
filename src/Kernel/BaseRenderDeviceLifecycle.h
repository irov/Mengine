#pragma once

#include "Interface/RenderDeviceLifecycleInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseRenderDeviceLifecycle
        : public RenderDeviceLifecycleInterface
    {
    public:
        BaseRenderDeviceLifecycle();
        ~BaseRenderDeviceLifecycle() override;

    protected:
        virtual void _onRenderDeviceCreate();
        virtual void _onRenderDeviceDestroy();
        virtual void _onRenderDeviceLostPrepare();
        virtual void _onRenderDeviceLostRestore();

    private:
        void onRenderDeviceCreate() override;
        void onRenderDeviceDestroy() override;
        void onRenderDeviceLostPrepare() override;
        void onRenderDeviceLostRestore() override;
    };
    //////////////////////////////////////////////////////////////////////////
}
