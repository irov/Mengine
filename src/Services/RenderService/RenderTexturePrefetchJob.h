#pragma once

#include "Interface/RenderTexturePrefetchInterface.h"

#include "ThreadTaskPrefetchTexture.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTexturePrefetchJob
        : public Factorable
    {
        DECLARE_FACTORABLE( RenderTexturePrefetchJob );

    public:
        RenderTexturePrefetchJob();
        ~RenderTexturePrefetchJob() override;

    public:
        const ContentInterfacePtr & getContent() const;
        uint32_t getCodecFlags() const;
        ERenderTexturePrefetchState getState() const;
        const RenderTextureInterfacePtr & getTexture() const;

    public:
        void initialize( const ContentInterfacePtr & _content, uint32_t _codecFlags );

    public:
        bool isPreparing() const;

    public:
        bool start();
        void update();
        void upload();
        void reject();
        void cancel();
        void stop();

    private:
        bool isCompatibleTexture_( const RenderTextureInterfacePtr & _texture, const RenderImageDesc & _desc ) const;

    private:
        ContentInterfacePtr m_content;
        uint32_t m_codecFlags;
        ERenderTexturePrefetchState m_state;
        ThreadTaskPrefetchTexturePtr m_task;
        PrefetchedRenderImageLoaderPtr m_imageLoader;
        RenderTextureInterfacePtr m_texture;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTexturePrefetchJob> RenderTexturePrefetchJobPtr;
    //////////////////////////////////////////////////////////////////////////
}
