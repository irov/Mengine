#pragma once

#include "MosaicInterface.h"

#include "MosaicAllocator.h"
#include "MosaicFontProvider.h"
#include "MosaicPlatformAdapter.h"
#include "MosaicRendererAdapter.h"

#include "Mosaic/GraphicsBridge.hpp"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    class MosaicService
        : public ServiceBase<MosaicServiceInterface>
    {
        DECLARE_FACTORABLE( MosaicService );

    public:
        MosaicService();
        ~MosaicService() override;

    protected:
        void _dependencyService() override;
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    protected:
        Mosaic::Context * getContext() const override;

    protected:
        const Mosaic::RenderMesh * prepareRenderMesh( const Mosaic::Frame & _frame ) override;

    protected:
        bool initializeGraphicsBridge_();

    protected:
        MosaicTextureHandle getTextureHandle( const RenderTextureInterfacePtr & _texture ) override;
        void releaseTextureHandle( MosaicTextureHandle _handle ) override;

    protected:
        MosaicProviderId addProvider( const UnknownMosaicRenderInterface::LambdaMosaicProvider & _provider ) override;
        void removeProvider( MosaicProviderId _id ) override;
        const VectorMosaicProviders & getProviders() const override;

    public:
        MosaicRendererAdapter * getRendererAdapter();
        MosaicFontProvider * getFontProvider();

    protected:
        void notifyRenderDeviceDestroy_();
        void notifyRenderDeviceLostPrepare_();
        void notifyRenderDeviceLostRestore_();

    protected:
        void loadSettings_();
        void saveSettings_();

    protected:
        Mosaic::Context * m_context;

        MosaicAllocator m_allocator;
        MosaicRendererAdapter m_renderer;
        MosaicPlatformAdapter m_platform;
        Mosaic::GraphicsBridge m_bridge;

        MosaicFontProvider * m_fontProvider;

        VectorMosaicProviders m_providers;

        MosaicProviderId m_providerEnumerator;

        UniqueId m_handlerId;
    };
}
