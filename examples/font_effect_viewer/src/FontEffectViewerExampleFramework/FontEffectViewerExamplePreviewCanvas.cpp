#include "FontEffectViewerExamplePreviewCanvas.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/MaterialEnumInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/Color.h"
#include "Kernel/PixelFormat.h"
#include "Kernel/ConstStringHelper.h"

#include "math/mat4.h"

namespace Mengine
{
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        class FontEffectViewerExamplePreviewCanvasCompositor
            : public Factorable
            , public Renderable
            , protected BaseRender
        {
            DECLARE_FACTORABLE( FontEffectViewerExamplePreviewCanvasCompositor );
            DECLARE_RENDERABLE();

        public:
            explicit FontEffectViewerExamplePreviewCanvasCompositor( const FontEffectViewerExamplePreviewCanvas * _owner )
                : m_owner( _owner )
            {
            }

        protected:
            void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override
            {
                const RenderMaterialInterfacePtr & material = m_owner->getCompositeMaterial();

                if( material == nullptr )
                {
                    return;
                }

                const RenderVertex2D * vertices = m_owner->getCompositeVertices();
                const RenderIndex * indices = m_owner->getCompositeIndices();

                _renderPipeline->addRenderObject( _context, material, nullptr, vertices, 4u, indices, 6u, nullptr, 0u, MENGINE_DOCUMENT_FACTORABLE );
            }

        protected:
            const FontEffectViewerExamplePreviewCanvas * m_owner;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<FontEffectViewerExamplePreviewCanvasCompositor> FontEffectViewerExamplePreviewCanvasCompositorPtr;
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectViewerExamplePreviewCanvas::FontEffectViewerExamplePreviewCanvas()
        : m_size( 1.f, 1.f )
        , m_zoom( 1.f )
    {
        for( uint32_t index = 0; index != 4; ++index )
        {
            m_compositeVertices[index].position = mt::vec3f( 0.f, 0.f, 0.f );
            m_compositeVertices[index].uv[0] = mt::vec2f( 0.f, 0.f );
            m_compositeVertices[index].uv[1] = mt::vec2f( 0.f, 0.f );
            m_compositeVertices[index].color = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectViewerExamplePreviewCanvas::~FontEffectViewerExamplePreviewCanvas()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExamplePreviewCanvas::setSize( uint32_t _width, uint32_t _height )
    {
        mt::vec2f size( (float)_width, (float)_height );

        if( m_size == size )
        {
            return true;
        }

        m_size = size;

        if( this->isActivate() == false )
        {
            return true;
        }

        this->clearRenderTarget_();

        if( this->createRenderTarget_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & FontEffectViewerExamplePreviewCanvas::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExamplePreviewCanvas::setZoom( float _zoom )
    {
        if( m_zoom == _zoom )
        {
            return;
        }

        m_zoom = _zoom;

        if( this->isActivate() == false )
        {
            return;
        }

        if( m_renderCamera == nullptr )
        {
            return;
        }

        this->updateCamera_();
    }
    //////////////////////////////////////////////////////////////////////////
    float FontEffectViewerExamplePreviewCanvas::getZoom() const
    {
        return m_zoom;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTargetInterfacePtr & FontEffectViewerExamplePreviewCanvas::getTarget() const
    {
        return m_sceneTarget;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderTextureInterfacePtr & FontEffectViewerExamplePreviewCanvas::getTexture() const
    {
        return m_sceneTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & FontEffectViewerExamplePreviewCanvas::getCompositeMaterial() const
    {
        return m_compositeMaterial;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertex2D * FontEffectViewerExamplePreviewCanvas::getCompositeVertices() const
    {
        return m_compositeVertices;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderIndex * FontEffectViewerExamplePreviewCanvas::getCompositeIndices() const
    {
        return m_compositeIndices;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExamplePreviewCanvas::_activate()
    {
        if( this->createRenderTarget_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExamplePreviewCanvas::_deactivate()
    {
        this->clearRenderTarget_();
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExamplePreviewCanvas::_dispose()
    {
        this->clearRenderTarget_();
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderInterfacePtr & FontEffectViewerExamplePreviewCanvas::makeTargetRender( const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _context );

        return m_compositeRender;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExamplePreviewCanvas::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExamplePreviewCanvas::updateCamera_()
    {
        float zoom = m_zoom > 0.001f ? m_zoom : 0.001f;

        Viewport fixedViewport( 0.f, 0.f, m_size.x, m_size.y );

        m_renderCamera->setProxyViewMatrix( true );
        m_renderCamera->setOrthogonalViewport( fixedViewport );
        m_renderCamera->setCameraScale( mt::vec2f( zoom, zoom ) );

        m_renderViewport->setViewport( fixedViewport );

        m_renderResolution->setContentResolution( Resolution( (uint32_t)m_size.x, (uint32_t)m_size.y ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExamplePreviewCanvas::createRenderTarget_()
    {
        uint32_t width = (uint32_t)m_size.x;
        uint32_t height = (uint32_t)m_size.y;

        if( width == 0u || height == 0u )
        {
            return false;
        }

        RenderTargetInterfacePtr sceneTarget = RENDER_SYSTEM()
            ->createRenderTargetTexture( width, height, PF_A8R8G8B8, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( sceneTarget, "preview canvas '%s' invalid create render target [%u, %u]"
            , this->getName().c_str()
            , width
            , height
        );

        RenderInterface * render = this->getRender();
        render->setRenderTarget( sceneTarget );

        RenderImageInterfacePtr sceneImage = RENDER_SYSTEM()
            ->createRenderImageTarget( sceneTarget, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( sceneImage, "preview canvas '%s' invalid create render image target"
            , this->getName().c_str()
        );

        RenderTextureInterfacePtr sceneTexture = RENDERTEXTURE_SERVICE()
            ->createRenderTexture( sceneImage, width, height, DF_IMAGE_NONE, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( sceneTexture, "preview canvas '%s' invalid create render texture [%u, %u]"
            , this->getName().c_str()
            , width
            , height
        );

        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
            ->getMaterial3( EM_TEXTURE_BLEND, PT_TRIANGLELIST, &sceneTexture, 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "preview canvas '%s' invalid get composite material"
            , this->getName().c_str()
        );

        if( m_renderCamera == nullptr )
        {
            RenderCameraOrthogonalPtr renderCamera = Helper::generateNodeFactorable<RenderCameraOrthogonal>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( renderCamera, "preview canvas '%s' invalid create RenderCameraOrthogonal"
                , this->getName().c_str()
            );

            this->addChild( renderCamera );

            m_renderCamera = renderCamera;
        }

        if( m_renderViewport == nullptr )
        {
            RenderViewportPtr renderViewport = Helper::generateNodeFactorable<RenderViewport>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( renderViewport, "preview canvas '%s' invalid create RenderViewport"
                , this->getName().c_str()
            );

            this->addChild( renderViewport );

            m_renderViewport = renderViewport;
        }

        if( m_renderResolution == nullptr )
        {
            RenderResolutionPtr renderResolution = Helper::makeFactorableUnique<RenderResolution>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( renderResolution, "preview canvas '%s' invalid create RenderResolution"
                , this->getName().c_str()
            );

            this->addChild( renderResolution );

            m_renderResolution = renderResolution;
        }

        render->setRenderCamera( m_renderCamera );
        render->setRenderViewport( m_renderViewport );
        render->setRenderResolution( m_renderResolution );

        this->updateCamera_();

        const mt::mat4f & wm = this->getWorldMatrix();

        mt::mul_v3_v2_m4( &m_compositeVertices[0].position, mt::vec2f( 0.f, 0.f ), wm );
        mt::mul_v3_v2_m4( &m_compositeVertices[1].position, mt::vec2f( m_size.x, 0.f ), wm );
        mt::mul_v3_v2_m4( &m_compositeVertices[2].position, mt::vec2f( m_size.x, m_size.y ), wm );
        mt::mul_v3_v2_m4( &m_compositeVertices[3].position, mt::vec2f( 0.f, m_size.y ), wm );

        const mt::uv4f & uv = sceneTarget->getUV();

        m_compositeVertices[0].uv[0] = uv.p0;
        m_compositeVertices[1].uv[0] = uv.p1;
        m_compositeVertices[2].uv[0] = uv.p2;
        m_compositeVertices[3].uv[0] = uv.p3;

        Color transparent( 1.f, 1.f, 1.f, 0.f );
        ColorValue_ARGB argb = transparent.getAsARGB();

        m_compositeVertices[0].color = argb;
        m_compositeVertices[1].color = argb;
        m_compositeVertices[2].color = argb;
        m_compositeVertices[3].color = argb;

        FontEffectViewerExamplePreviewCanvasCompositorPtr compositor = Helper::makeFactorableUnique<FontEffectViewerExamplePreviewCanvasCompositor>( MENGINE_DOCUMENT_FACTORABLE, this );

        MENGINE_ASSERTION_MEMORY_PANIC( compositor, "preview canvas '%s' invalid make compositor"
            , this->getName().c_str()
        );

        m_sceneTarget = sceneTarget;
        m_sceneTexture = sceneTexture;
        m_compositeMaterial = material;
        m_compositeRender = compositor->getRender();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExamplePreviewCanvas::clearRenderTarget_()
    {
        m_compositeRender = nullptr;
        m_compositeMaterial = nullptr;
        m_sceneTexture = nullptr;
        m_sceneTarget = nullptr;

        RenderInterface * render = this->getRender();
        render->setRenderTarget( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
