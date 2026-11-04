#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "MetalRenderProgram.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderMaterialStageCache
        : public RenderMaterialStageCacheInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderMaterialStageCache );

    public:
        MetalRenderMaterialStageCache();
        ~MetalRenderMaterialStageCache() override;

    public:
        MENGINE_INLINE const MetalBlendStateKey & getBlendKey() const;
        MENGINE_INLINE bool getDepthBufferTestEnable() const;
        MENGINE_INLINE bool getDepthBufferWriteEnable() const;
        MENGINE_INLINE const RenderTextureStage & getTextureStage( uint32_t _stageId ) const;

    public:
        void initialize( const RenderMaterialStage * _stage );

    protected:
        MetalBlendStateKey m_blendKey;

        bool m_depthBufferTestEnable;
        bool m_depthBufferWriteEnable;

        RenderTextureStage m_textureStages[MENGINE_MAX_TEXTURE_STAGES];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderMaterialStageCache, RenderMaterialStageCacheInterface> MetalRenderMaterialStageCachePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const MetalBlendStateKey & MetalRenderMaterialStageCache::getBlendKey() const
    {
        return m_blendKey;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool MetalRenderMaterialStageCache::getDepthBufferTestEnable() const
    {
        return m_depthBufferTestEnable;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool MetalRenderMaterialStageCache::getDepthBufferWriteEnable() const
    {
        return m_depthBufferWriteEnable;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderTextureStage & MetalRenderMaterialStageCache::getTextureStage( uint32_t _stageId ) const
    {
        return m_textureStages[_stageId];
    }
    //////////////////////////////////////////////////////////////////////////
}
