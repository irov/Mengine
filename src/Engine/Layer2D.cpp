#include "Layer2D.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Layer2DRenderTarget.h"

#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/Scene.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Layer2D::Layer2D()
        : m_size( 0.f, 0.f )
        , m_viewport( 0.f, 0.f, 0.f, 0.f )
        , m_invalidateVerticesImageMaskColor( true )
        , m_invalidateVerticesImageMaskWM( true )
        , m_hasViewport( false )
        , m_hasImageMask( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Layer2D::~Layer2D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Layer2D::_activate()
    {
        if( Layer::_activate() == false )
        {
            return false;
        }

        if( m_hasViewport == true )
        {
            this->createViewport_();
        }

        if( m_hasImageMask == true )
        {
            if( this->createRenderTarget_() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_deactivate()
    {
        if( m_hasViewport == true )
        {
            this->clearViewport_();
        }

        if( m_hasImageMask == true )
        {
            this->clearRenderTarget_();
        }

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_dispose()
    {
        m_resourceImageMask = nullptr;

        Layer::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::setSize( const mt::vec2f & _size )
    {
        m_size = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Layer2D::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::setViewport( const Viewport & _viewport )
    {
        if( m_viewport.equalViewport( _viewport, 0.f ) == true && m_hasViewport == true )
        {
            return;
        }

        m_hasViewport = true;

        m_viewport = _viewport;

        this->createViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Layer2D::getViewport( Viewport * const _viewport ) const
    {
        *_viewport = m_viewport;

        return m_hasViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::createViewport_()
    {
        if( m_renderCamera == nullptr )
        {
            RenderCameraOrthogonalPtr renderCamera = Helper::generateNodeFactorable<RenderCameraOrthogonal>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( renderCamera, "name '%s' invalid create RenderCameraOrthogonal"
                , this->getName().c_str()
            );

            const ConstString & name = this->getName();

            renderCamera->setName( name );

            this->addChild( renderCamera );

            m_renderCamera = renderCamera;
        }

        if( m_renderViewport == nullptr )
        {
            RenderViewportPtr renderViewport = Helper::generateNodeFactorable<RenderViewport>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( renderViewport, "name '%s' invalid create RenderViewport"
                , this->getName().c_str()
            );

            const ConstString & name = this->getName();

            renderViewport->setName( name );

            this->addChild( renderViewport );

            m_renderViewport = renderViewport;
        }

        m_renderCamera->setProxyViewMatrix( true );
        m_renderCamera->setOrthogonalViewport( m_viewport );

        m_renderViewport->setViewport( m_viewport );

        RenderInterface * render = this->getRender();

        render->setRenderCamera( m_renderCamera );
        render->setRenderViewport( m_renderViewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::removeViewport()
    {
        if( m_hasViewport == false )
        {
            return;
        }

        m_hasViewport = false;

        this->clearViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::clearViewport_()
    {
        m_renderViewport = nullptr;
        m_renderCamera = nullptr;

        RenderInterface * render = this->getRender();

        render->setRenderCamera( nullptr );
        render->setRenderViewport( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Layer2D::setImageMask( const ResourceImagePtr & _resourceImageMask )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _resourceImageMask, "image mask is nullptr" );

        if( m_resourceImageMask == _resourceImageMask && m_hasImageMask == true )
        {
            return true;
        }

        m_resourceImageMask = _resourceImageMask;

        if( this->createRenderTarget_() == false )
        {
            LOGGER_ERROR( "layer2d '%s' invalid create render target"
                , this->getName().c_str()
            );

            m_resourceImageMask = nullptr;

            return false;
        }

        m_hasImageMask = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Layer2D::getImageMask( ResourceImagePtr * const _resourceImageMask ) const
    {
        *_resourceImageMask = m_resourceImageMask;

        return m_hasImageMask;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::removeImageMask()
    {
        if( m_hasImageMask == false )
        {
            return;
        }

        m_hasImageMask = false;

        this->clearRenderTarget_();

        m_resourceImageMask = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Layer2D::createRenderTarget_()
    {
        uint32_t width = (uint32_t)m_size.x;
        uint32_t height = (uint32_t)m_size.y;

        RenderTargetInterfacePtr renderTarget = RENDER_SYSTEM()
            ->createRenderTargetTexture( width, height, PF_A8R8G8B8, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTarget, "invalid create render target texture [%u, %u]"
            , width
            , height
        );

        RenderInterface * render = this->getRender();
        render->setRenderTarget( renderTarget );

        RenderImageInterfacePtr renderTargetImage = RENDER_SYSTEM()
            ->createRenderImageTarget( renderTarget, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetImage, "invalid create render target image" );

        RenderTextureInterfacePtr renderTargetTexture = RENDERTEXTURE_SERVICE()
            ->createRenderTexture( renderTargetImage, width, height, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetTexture, "invalid create render texture [%u, %u]"
            , width
            , height
        );

        if( m_resourceImageMask->compile() == false )
        {
            LOGGER_ERROR( "layer2d '%s' invalid compile resource image mask '%s'"
                , this->getName().c_str()
                , m_resourceImageMask->getName().c_str()
            );

            return false;
        }

        const RenderTextureInterfacePtr & renderTargetTextureMask = m_resourceImageMask->getTexture();

        RenderTextureInterfacePtr texures[2];
        texures[0] = renderTargetTexture;
        texures[1] = renderTargetTextureMask;

        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
            ->getMaterial3( EM_TEXTURE_ALPHAMASK_BLEND, PT_TRIANGLELIST, texures, 2, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "invalid get material" );

        const mt::uv4f & uv = renderTarget->getUV();

        m_verticesImageMaskWM[0].uv[0] = uv.p0;
        m_verticesImageMaskWM[1].uv[0] = uv.p1;
        m_verticesImageMaskWM[2].uv[0] = uv.p2;
        m_verticesImageMaskWM[3].uv[0] = uv.p3;

        const mt::uv4f & uv_mask = renderTargetTextureMask->getUV();

        m_verticesImageMaskWM[0].uv[1] = uv_mask.p0;
        m_verticesImageMaskWM[1].uv[1] = uv_mask.p1;
        m_verticesImageMaskWM[2].uv[1] = uv_mask.p2;
        m_verticesImageMaskWM[3].uv[1] = uv_mask.p3;

        Layer2DRenderTargetPtr layer2DRenderTarget = Helper::makeFactorableUnique<Layer2DRenderTarget>( MENGINE_DOCUMENT_FACTORABLE, this );

        MENGINE_ASSERTION_MEMORY_PANIC( layer2DRenderTarget, "invalid make render target" );

        m_renderTarget = layer2DRenderTarget->getRender();

        m_materialImageMask = material;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::clearRenderTarget_()
    {
        m_materialImageMask = nullptr;

        m_resourceImageMask->release();

        m_renderTarget = nullptr;

        this->setRenderTarget( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderInterfacePtr & Layer2D::makeTargetRender( const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _context );

        return m_renderTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::updateVerticesImageMaskWM() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::mul_v3_v2_m4( &m_verticesImageMaskWM[0].position, mt::vec2f( 0.f, 0.f ), wm );
        mt::mul_v3_v2_m4( &m_verticesImageMaskWM[1].position, mt::vec2f( m_size.x, 0.f ), wm );
        mt::mul_v3_v2_m4( &m_verticesImageMaskWM[2].position, mt::vec2f( m_size.x, m_size.y ), wm );
        mt::mul_v3_v2_m4( &m_verticesImageMaskWM[3].position, mt::vec2f( 0.f, m_size.y ), wm );
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::updateVerticesImageMaskColor() const
    {
        Color color;
        this->calcTotalColor( &color );

        ColorValue_ARGB argb = color.getAsARGB();

        m_verticesImageMaskWM[0].color = argb;
        m_verticesImageMaskWM[1].color = argb;
        m_verticesImageMaskWM[2].color = argb;
        m_verticesImageMaskWM[3].color = argb;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_invalidateColor() const
    {
        m_invalidateVerticesImageMaskColor = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_invalidateWorldMatrix() const
    {
        m_invalidateVerticesImageMaskWM = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderResolutionInterfacePtr & Layer2D::getPickerResolution() const
    {
        const RenderResolutionInterfacePtr & resolution = this->getRenderResolution();

        return resolution;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderViewportInterfacePtr & Layer2D::getPickerViewport() const
    {
        const RenderViewportInterfacePtr & viewport = this->getRenderViewport();

        return viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderCameraInterfacePtr & Layer2D::getPickerCamera() const
    {
        const RenderCameraInterfacePtr & camera = this->getRenderCamera();

        return camera;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderScissorInterfacePtr & Layer2D::getPickerScissor() const
    {
        const RenderScissorInterfacePtr & scissor = this->getRenderScissor();

        return scissor;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTransformationInterfacePtr & Layer2D::getPickerTransformation() const
    {
        const RenderTransformationInterfacePtr & transformation = this->getRenderTransformation();

        return transformation;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTargetInterfacePtr & Layer2D::getPickerTarget() const
    {
        const RenderTargetInterfacePtr & target = this->getRenderTarget();

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
}
