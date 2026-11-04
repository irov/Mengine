#include "MetalRenderMaterialStageCache.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderMaterialStageCache::MetalRenderMaterialStageCache()
        : m_depthBufferTestEnable( false )
        , m_depthBufferWriteEnable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderMaterialStageCache::~MetalRenderMaterialStageCache()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderMaterialStageCache::initialize( const RenderMaterialStage * _stage )
    {
        m_blendKey.alphaBlend = _stage->alphaBlendEnable;
        m_blendKey.blendSrc = _stage->blendSrc;
        m_blendKey.blendDst = _stage->blendDst;
        m_blendKey.blendOp = _stage->blendOp;
        m_blendKey.blendSeparateSrc = _stage->separateAlphaBlendSrc;
        m_blendKey.blendSeparateDst = _stage->separateAlphaBlendDst;
        m_blendKey.blendSeparateOp = _stage->separateAlphaBlendOp;
        m_blendKey.blendSeparate = _stage->separateAlphaBlendEnable;

        m_depthBufferTestEnable = _stage->depthBufferTestEnable;
        m_depthBufferWriteEnable = _stage->depthBufferWriteEnable;

        for( uint32_t stageId = 0; stageId != MENGINE_MAX_TEXTURE_STAGES; ++stageId )
        {
            m_textureStages[stageId] = _stage->textureStages[stageId];
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
