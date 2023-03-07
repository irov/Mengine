#pragma once

#include "ImGUIInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    class ImGUIService
        : public ServiceBase<ImGUIServiceInterface>
    {
        DECLARE_FACTORABLE( ImGUIService );

    public:
        ImGUIService();
        ~ImGUIService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        const ImGUIRenderProviderInterfacePtr & getRenderProvider() const override;

    protected:
        void notifyPlatformAtachWindow_();
        void notifyPlatformDetachWindow_();
        void notifyCreateRenderWindow_();
        void notifyRenderDeviceLostPrepare_();
        void notifyRenderDeviceLostRestore_();

    protected:
        UniqueId m_handlerId;

        ImGUIRenderProviderInterfacePtr m_renderProvider;
    };
}