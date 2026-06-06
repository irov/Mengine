#include "PostProcessFx3D.h"

#include "PostProcessFx3DRenderTarget.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Color.h"
#include "Kernel/PixelFormat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PostProcessFx3D::PostProcessFx3D()
        : m_materialName( STRINGIZE_STRING_LOCAL( "Material_PostProcessFx" ) )
        , m_size( 1.f, 1.f )
        , m_exposure( 1.f )
        , m_vignetteStrength( 0.4f )
        , m_vignetteSoftness( 0.5f )
        , m_chromaticOffset( 0.f )
        , m_hitFlashColor( 1.f, 0.f, 0.f )
        , m_hitFlashIntensity( 0.f )
        , m_invalidateVerticesWM( true )
        , m_invalidateVerticesColor( true )
        , m_pixelUniformsRegistered( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PostProcessFx3D::~PostProcessFx3D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::setSize( const mt::vec2f & _size )
    {
        if( m_size == _size )
        {
            return;
        }

        m_size = _size;

        m_invalidateVerticesWM = true;

        if( this->isActivate() == true )
        {
            this->clearRenderTarget_();
            this->createRenderTarget_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & PostProcessFx3D::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::setMaterialName( const ConstString & _materialName )
    {
        if( m_materialName == _materialName )
        {
            return;
        }

        m_materialName = _materialName;

        if( this->isActivate() == true )
        {
            this->clearRenderTarget_();
            this->createRenderTarget_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & PostProcessFx3D::getMaterialName() const
    {
        return m_materialName;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::setExposure( float _exposure )
    {
        m_exposure = _exposure;
    }
    //////////////////////////////////////////////////////////////////////////
    float PostProcessFx3D::getExposure() const
    {
        return m_exposure;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::setVignette( float _strength, float _softness )
    {
        m_vignetteStrength = _strength;
        m_vignetteSoftness = _softness;
    }
    //////////////////////////////////////////////////////////////////////////
    float PostProcessFx3D::getVignetteStrength() const
    {
        return m_vignetteStrength;
    }
    //////////////////////////////////////////////////////////////////////////
    float PostProcessFx3D::getVignetteSoftness() const
    {
        return m_vignetteSoftness;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::setChromaticOffset( float _pixels )
    {
        m_chromaticOffset = _pixels;
    }
    //////////////////////////////////////////////////////////////////////////
    float PostProcessFx3D::getChromaticOffset() const
    {
        return m_chromaticOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::setHitFlash( const mt::vec3f & _color, float _intensity )
    {
        m_hitFlashColor = _color;
        m_hitFlashIntensity = _intensity;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & PostProcessFx3D::getHitFlashColor() const
    {
        return m_hitFlashColor;
    }
    //////////////////////////////////////////////////////////////////////////
    float PostProcessFx3D::getHitFlashIntensity() const
    {
        return m_hitFlashIntensity;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertex2D * PostProcessFx3D::getVerticesWM() const
    {
        if( m_invalidateVerticesWM == true )
        {
            m_invalidateVerticesWM = false;

            this->updateVerticesWM_();
        }

        if( m_invalidateVerticesColor == true )
        {
            m_invalidateVerticesColor = false;

            this->updateVerticesColor_();
        }

        return m_verticesWM;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & PostProcessFx3D::getMaterial() const
    {
        return m_material;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderProgramVariableInterfacePtr & PostProcessFx3D::getProgramVariable() const
    {
        this->packPixelUniforms_();

        return m_programVariable;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderIndex * PostProcessFx3D::getIndices() const
    {
        return m_indices;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PostProcessFx3D::_activate()
    {
        if( this->createRenderTarget_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::_deactivate()
    {
        this->clearRenderTarget_();
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::_dispose()
    {
        this->clearRenderTarget_();
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderInterfacePtr & PostProcessFx3D::makeTargetRender( const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _context );

        return m_renderTargetRender;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::_invalidateColor() const
    {
        m_invalidateVerticesColor = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::_invalidateWorldMatrix() const
    {
        m_invalidateVerticesWM = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PostProcessFx3D::createRenderTarget_()
    {
        uint32_t width = (uint32_t)m_size.x;
        uint32_t height = (uint32_t)m_size.y;

        if( width == 0u || height == 0u )
        {
            LOGGER_ERROR( "post-process fx '%s' invalid size [%u, %u]"
                , this->getName().c_str()
                , width
                , height
            );

            return false;
        }

        RenderTargetInterfacePtr sceneTarget = RENDER_SYSTEM()
            ->createRenderTargetTexture( width, height, PF_A8R8G8B8, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( sceneTarget, "post-process fx '%s' invalid create render target [%u, %u]"
            , this->getName().c_str()
            , width
            , height
        );

        RenderInterface * render = this->getRender();
        render->setRenderTarget( sceneTarget );

        RenderImageInterfacePtr sceneImage = RENDER_SYSTEM()
            ->createRenderImageTarget( sceneTarget, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( sceneImage, "post-process fx '%s' invalid create render image target"
            , this->getName().c_str()
        );

        RenderTextureInterfacePtr sceneTexture = RENDERTEXTURE_SERVICE()
            ->createRenderTexture( sceneImage, width, height, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( sceneTexture, "post-process fx '%s' invalid create render texture [%u, %u]"
            , this->getName().c_str()
            , width
            , height
        );

        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
            ->getMaterial( m_materialName, PT_TRIANGLELIST, &sceneTexture, 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "post-process fx '%s' invalid get material '%s'"
            , this->getName().c_str()
            , m_materialName.c_str()
        );

        RenderProgramVariableInterfacePtr programVariable = RENDER_SYSTEM()
            ->createProgramVariableDynamic( 0, 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( programVariable, "post-process fx '%s' invalid create program variable"
            , this->getName().c_str()
        );

        const mt::uv4f & uv = sceneTarget->getUV();
        m_verticesWM[0].uv[0] = uv.p0;
        m_verticesWM[1].uv[0] = uv.p1;
        m_verticesWM[2].uv[0] = uv.p2;
        m_verticesWM[3].uv[0] = uv.p3;
        m_verticesWM[0].uv[1] = uv.p0;
        m_verticesWM[1].uv[1] = uv.p1;
        m_verticesWM[2].uv[1] = uv.p2;
        m_verticesWM[3].uv[1] = uv.p3;

        PostProcessFx3DRenderTargetPtr renderProxy = Helper::makeFactorableUnique<PostProcessFx3DRenderTarget>( MENGINE_DOCUMENT_FACTORABLE, this );

        MENGINE_ASSERTION_MEMORY_PANIC( renderProxy, "post-process fx '%s' invalid make overlay render"
            , this->getName().c_str()
        );

        m_sceneTarget = sceneTarget;
        m_sceneTexture = sceneTexture;
        m_material = material;
        m_programVariable = programVariable;
        m_renderTargetRender = renderProxy->getRender();

        m_invalidateVerticesWM = true;
        m_invalidateVerticesColor = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::clearRenderTarget_()
    {
        m_renderTargetRender = nullptr;
        m_programVariable = nullptr;
        m_pixelUniformsRegistered = false;
        m_material = nullptr;
        m_sceneTexture = nullptr;
        m_sceneTarget = nullptr;

        RenderInterface * render = this->getRender();
        render->setRenderTarget( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::updateVerticesWM_() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::mul_v3_v2_m4( &m_verticesWM[0].position, mt::vec2f( 0.f,      0.f      ), wm );
        mt::mul_v3_v2_m4( &m_verticesWM[1].position, mt::vec2f( m_size.x, 0.f      ), wm );
        mt::mul_v3_v2_m4( &m_verticesWM[2].position, mt::vec2f( m_size.x, m_size.y ), wm );
        mt::mul_v3_v2_m4( &m_verticesWM[3].position, mt::vec2f( 0.f,      m_size.y ), wm );
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::updateVerticesColor_() const
    {
        Color color;
        this->calcTotalColor( &color );

        ColorValue_ARGB argb = color.getAsARGB();

        m_verticesWM[0].color = argb;
        m_verticesWM[1].color = argb;
        m_verticesWM[2].color = argb;
        m_verticesWM[3].color = argb;
    }
    //////////////////////////////////////////////////////////////////////////
    void PostProcessFx3D::packPixelUniforms_() const
    {
        if( m_programVariable == nullptr )
        {
            return;
        }

        float texelU = (m_size.x > 0.f) ? (1.f / m_size.x) : 0.f;
        float texelV = (m_size.y > 0.f) ? (1.f / m_size.y) : 0.f;

        float buffer[12];
        buffer[0] = m_exposure;
        buffer[1] = m_vignetteStrength;
        buffer[2] = m_vignetteSoftness;
        buffer[3] = m_chromaticOffset;

        buffer[4] = m_hitFlashColor.x;
        buffer[5] = m_hitFlashColor.y;
        buffer[6] = m_hitFlashColor.z;
        buffer[7] = m_hitFlashIntensity;

        buffer[8]  = 0.5f;
        buffer[9]  = 0.5f;
        buffer[10] = texelU;
        buffer[11] = texelV;

        if( m_pixelUniformsRegistered == false )
        {
            m_programVariable->setPixelVariables( "PostFx", 0u, buffer, 4u, 3u );

            m_pixelUniformsRegistered = true;
        }
        else
        {
            m_programVariable->updatePixelVariables( 0u, buffer, 4u, 3u );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
