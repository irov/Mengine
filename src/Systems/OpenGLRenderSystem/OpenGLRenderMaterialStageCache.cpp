#include "OpenGLRenderMaterialStageCache.h"

#include "OpenGLRenderEnum.h"
#include "OpenGLRenderErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderMaterialStageCache::OpenGLRenderMaterialStageCache()
        : m_alphaBlendEnable( false )
        , m_separateAlphaBlendEnable( false )
        , m_blendSrc( GL_ONE )
        , m_blendDst( GL_ZERO )
        , m_blendOp( GL_FUNC_ADD )
        , m_separateAlphaBlendSrc( GL_ONE )
        , m_separateAlphaBlendDst( GL_ZERO )
        , m_separateAlphaBlendOp( GL_FUNC_ADD )
        , m_depthBufferTestEnable( false )
        , m_depthBufferWriteEnable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderMaterialStageCache::~OpenGLRenderMaterialStageCache()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderMaterialStageCache::initialize( const RenderMaterialStage * _stage )
    {
        m_alphaBlendEnable = _stage->alphaBlendEnable;
        m_separateAlphaBlendEnable = _stage->separateAlphaBlendEnable;

        m_blendSrc = Helper::toGLBlendFactor( _stage->blendSrc );
        m_blendDst = Helper::toGLBlendFactor( _stage->blendDst );
        m_blendOp = Helper::toGLBlendOp( _stage->blendOp );

        m_separateAlphaBlendSrc = Helper::toGLBlendFactor( _stage->separateAlphaBlendSrc );
        m_separateAlphaBlendDst = Helper::toGLBlendFactor( _stage->separateAlphaBlendDst );
        m_separateAlphaBlendOp = Helper::toGLBlendOp( _stage->separateAlphaBlendOp );

        m_depthBufferTestEnable = _stage->depthBufferTestEnable;
        m_depthBufferWriteEnable = _stage->depthBufferWriteEnable;

        for( uint32_t stageId = 0; stageId != MENGINE_MAX_TEXTURE_STAGES; ++stageId )
        {
            const RenderTextureStage & textureStage = _stage->textureStages[stageId];

            OpenGLTextureStageDesc & desc = m_textureStages[stageId];

            desc.minFilter = Helper::toGLMinFilter( textureStage.minification, textureStage.mipmap );
            desc.magFilter = Helper::toGLMagFilter( textureStage.magnification );
            desc.wrapS = Helper::toGLAddressMode( textureStage.addressU );
            desc.wrapT = Helper::toGLAddressMode( textureStage.addressV );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderMaterialStageCache::apply() const
    {
        if( m_alphaBlendEnable == true )
        {
            MENGINE_GLCALL( glEnable, (GL_BLEND) );
        }
        else
        {
            MENGINE_GLCALL( glDisable, (GL_BLEND) );
        }

        if( m_separateAlphaBlendEnable == false )
        {
            MENGINE_GLCALL( glBlendFunc, (m_blendSrc, m_blendDst) );

#if defined(MENGINE_RENDER_OPENGL_ES)
            MENGINE_GLCALL( glBlendEquation, (m_blendOp) );
#else
            MENGINE_GLCALL( glBlendEquation_, (m_blendOp) );
#endif
        }
        else
        {
#if defined(MENGINE_RENDER_OPENGL_ES)
            MENGINE_GLCALL( glBlendFuncSeparate, (m_blendSrc, m_blendDst, m_separateAlphaBlendSrc, m_separateAlphaBlendDst) );
            MENGINE_GLCALL( glBlendEquationSeparate, (m_blendOp, m_separateAlphaBlendOp) );
#else
            MENGINE_GLCALL( glBlendFuncSeparate_, (m_blendSrc, m_blendDst, m_separateAlphaBlendSrc, m_separateAlphaBlendDst) );
            MENGINE_GLCALL( glBlendEquationSeparate_, (m_blendOp, m_separateAlphaBlendOp) );
#endif
        }

        if( m_depthBufferTestEnable == true )
        {
            MENGINE_GLCALL( glEnable, (GL_DEPTH_TEST) );
        }
        else
        {
            MENGINE_GLCALL( glDisable, (GL_DEPTH_TEST) );
        }

        if( m_depthBufferWriteEnable == true )
        {
            MENGINE_GLCALL( glDepthMask, (GL_TRUE) );
        }
        else
        {
            MENGINE_GLCALL( glDepthMask, (GL_FALSE) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
