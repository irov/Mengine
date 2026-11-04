#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "OpenGLRenderExtension.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct OpenGLTextureStageDesc
    {
        GLenum minFilter;
        GLenum magFilter;
        GLenum wrapS;
        GLenum wrapT;
    };
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderMaterialStageCache
        : public RenderMaterialStageCacheInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( OpenGLRenderMaterialStageCache );

    public:
        OpenGLRenderMaterialStageCache();
        ~OpenGLRenderMaterialStageCache() override;

    public:
        MENGINE_INLINE bool getDepthBufferWriteEnable() const;
        MENGINE_INLINE const OpenGLTextureStageDesc & getTextureStage( uint32_t _stageId ) const;

    public:
        void initialize( const RenderMaterialStage * _stage );

    public:
        void apply() const;

    protected:
        bool m_alphaBlendEnable;
        bool m_separateAlphaBlendEnable;

        GLenum m_blendSrc;
        GLenum m_blendDst;
        GLenum m_blendOp;

        GLenum m_separateAlphaBlendSrc;
        GLenum m_separateAlphaBlendDst;
        GLenum m_separateAlphaBlendOp;

        bool m_depthBufferTestEnable;
        bool m_depthBufferWriteEnable;

        OpenGLTextureStageDesc m_textureStages[MENGINE_MAX_TEXTURE_STAGES];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderMaterialStageCache, RenderMaterialStageCacheInterface> OpenGLRenderMaterialStageCachePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool OpenGLRenderMaterialStageCache::getDepthBufferWriteEnable() const
    {
        return m_depthBufferWriteEnable;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const OpenGLTextureStageDesc & OpenGLRenderMaterialStageCache::getTextureStage( uint32_t _stageId ) const
    {
        return m_textureStages[_stageId];
    }
    //////////////////////////////////////////////////////////////////////////
}
