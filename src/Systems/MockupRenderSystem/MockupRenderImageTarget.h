#pragma once

#include "Interface/RenderImageInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class MockupRenderTargetTexture> MockupRenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderImageTarget
        : public RenderImageInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MockupRenderImageTarget );

    public:
        MockupRenderImageTarget();
        ~MockupRenderImageTarget() override;

    public:
        void initialize( const MockupRenderTargetTexturePtr & _target );
        void finalize();

    protected:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWMipmaps() const override;

        EPixelFormat getHWPixelFormat() const override;

    protected:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    protected:
        bool getUpscalePow2() const override;

    public:
        RenderImageLockedInterfacePtr lock( uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful ) override;

    protected:
        MockupRenderTargetTexturePtr m_target;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderImageTarget> MockupRenderImageTargetPtr;
    //////////////////////////////////////////////////////////////////////////
}
