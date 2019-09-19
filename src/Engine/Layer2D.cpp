#include "Layer2D.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/Scene.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
        if( m_hasViewport == true )
        {
            return;
        }

        m_viewport = _viewport;

        m_hasViewport = true;

        this->createViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::createViewport_()
    {
        if( m_renderCamera == nullptr )
        {
            m_renderCamera = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderCameraOrthogonal" ), MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC_VOID( m_renderCamera, "name '%s' invalid create Camera2D"
                , this->getName().c_str()
            );

            this->addChild( m_renderCamera );
        }

        if( m_renderViewport == nullptr )
        {
            m_renderViewport = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderViewport" ), MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC_VOID( m_renderViewport, "name '%s' invalid create RenderViewport"
                , this->getName().c_str()
            );

            this->addChild( m_renderViewport );
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
        MENGINE_ASSERTION_MEMORY_PANIC( _resourceImageMask, false, "image mask is null" );

        m_resourceImageMask = _resourceImageMask;

        if( this->createRenderTarget_() == false )
        {
            LOGGER_ERROR( "invalid create render target" );

            return false;
        }

        m_hasImageMask = true;

        return true;
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
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class RenderLayer2DTarget
            : public Factorable
            , public BaseRender
        {
        public:
            RenderLayer2DTarget( Layer2D * _layer )
                : m_layer( _layer )
            {
            }

            ~RenderLayer2DTarget() override
            {
            }

        protected:
            void render( const RenderContext * _context ) const override
            {
                const RenderVertex2D * verticesImageMask = m_layer->getVerticesImageMaskWM();
                const RenderMaterialInterfacePtr & materialImageMask = m_layer->getMaterialImageMask();

                const mt::box2f * bb = m_layer->getBoundingBox();

                this->addRenderQuad( _context, materialImageMask, verticesImageMask, 4, bb, false, MENGINE_DOCUMENT_FUNCTION );
            }

        protected:
            Layer2D * m_layer;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool Layer2D::createRenderTarget_()
    {
        RenderTargetInterfacePtr renderTarget = RENDER_SYSTEM()
            ->createRenderTargetTexture( (uint32_t)m_size.x, (uint32_t)m_size.y, 3, PF_A8R8G8B8, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTarget, false, "invalid create render target texture [%f, %f]"
            , m_size.x
            , m_size.y
        );

        RenderInterface * render = this->getRender();
        render->setRenderTarget( renderTarget );

        RenderImageInterfacePtr renderTargetImage = RENDER_SYSTEM()
            ->createRenderTargetImage( renderTarget, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetImage, false, "invalid create render target image" );

        RenderTextureInterfacePtr renderTargetTexture = RENDERTEXTURE_SERVICE()
            ->createRenderTexture( renderTargetImage, (uint32_t)m_size.x, (uint32_t)m_size.y, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( renderTargetTexture, false, "invalid create render texture [%f, %f]"
            , m_size.x
            , m_size.y
        );

        if( m_resourceImageMask->compile() == false )
        {
            LOGGER_ERROR( "'%s' invalid compile resource image mask '%s'"
                , this->getName().c_str()
                , m_resourceImageMask->getName().c_str()
            );

            return false;
        }

        const RenderTextureInterfacePtr & renderTargetTextureMask = m_resourceImageMask->getTexture();

        RenderTextureInterfacePtr texures[2];
        texures[0] = renderTargetTexture;
        texures[1] = renderTargetTextureMask;

        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
            ->getMaterial3( EM_TEXTURE_ALPHAMASK_BLEND, PT_TRIANGLELIST, 2, texures, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( material, false, "invalid get material" );

        m_materialImageMask = material;

        float hwWidthInv = renderTarget->getHWWidthInv();
        float hwHeightInv = renderTarget->getHWHeightInv();

        float uv_width = m_size.x * hwWidthInv;
        float uv_height = m_size.y * hwHeightInv;

        m_verticesImageMaskWM[0].uv[0] = mt::vec2f( 0.f, 0.f );
        m_verticesImageMaskWM[1].uv[0] = mt::vec2f( uv_width, 0.f );
        m_verticesImageMaskWM[2].uv[0] = mt::vec2f( uv_width, uv_height );
        m_verticesImageMaskWM[3].uv[0] = mt::vec2f( 0.f, uv_height );

        const mt::uv4f & uv_mask = renderTargetTextureMask->getUV();

        m_verticesImageMaskWM[0].uv[1] = uv_mask.p0;
        m_verticesImageMaskWM[1].uv[1] = uv_mask.p1;
        m_verticesImageMaskWM[2].uv[1] = uv_mask.p2;
        m_verticesImageMaskWM[3].uv[1] = uv_mask.p3;

        m_renderTarget = Helper::makeFactorableUnique<RenderLayer2DTarget>( this );

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

        mt::mul_v3_v2_m4( m_verticesImageMaskWM[0].position, mt::vec2f( 0.f, 0.f ), wm );
        mt::mul_v3_v2_m4( m_verticesImageMaskWM[1].position, mt::vec2f( m_size.x, 0.f ), wm );
        mt::mul_v3_v2_m4( m_verticesImageMaskWM[2].position, mt::vec2f( m_size.x, m_size.y ), wm );
        mt::mul_v3_v2_m4( m_verticesImageMaskWM[3].position, mt::vec2f( 0.f, m_size.y ), wm );
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::updateVerticesImageMaskColor() const
    {
        Color color;
        this->calcTotalColor( color );

        uint32_t argb = color.getAsARGB();

        m_verticesImageMaskWM[0].color = argb;
        m_verticesImageMaskWM[1].color = argb;
        m_verticesImageMaskWM[2].color = argb;
        m_verticesImageMaskWM[3].color = argb;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_invalidateColor()
    {
        m_invalidateVerticesImageMaskColor = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        m_invalidateVerticesImageMaskWM = true;
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
}
