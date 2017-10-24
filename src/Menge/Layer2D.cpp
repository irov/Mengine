#	include "Layer2D.h"

#	include "Kernel/RenderCameraOrthogonal.h"

#	include "Kernel/Scene.h"

#	include "Kernel/RenderViewport.h"

#	include "Interface/NodeInterface.h"
#   include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer2D::Layer2D()
        : m_size(0.f, 0.f)
		, m_viewport(0.f, 0.f, 0.f, 0.f)
		, m_renderCamera(nullptr)
        , m_renderViewport( nullptr )
		, m_hasViewport(false)
        , m_hasImageMask(false)		
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

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_deactivate()
    {
        if( m_hasViewport == true )
        {
            this->clearViewport_();
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
		m_viewport = _viewport;
		
		m_hasViewport = true;

        this->createViewport_();
	}
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::createViewport_()
    {
		if( m_renderCamera == nullptr )
		{
			m_renderCamera = NODE_SERVICE( m_serviceProvider )
				->createNodeT<RenderCameraOrthogonal *>( CONST_STRING( m_serviceProvider, RenderCameraOrthogonal ) );

			if( m_renderCamera == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Layer2D::createRenderViewport_ %s invalid create Camera2D"
					, this->getName().c_str()
					);

				return;
			}

			this->addChild( m_renderCamera );
		}

		if( m_renderViewport == nullptr )
		{
			m_renderViewport = NODE_SERVICE( m_serviceProvider )
				->createNodeT<RenderViewport *>( CONST_STRING( m_serviceProvider, RenderViewport ) );

			if( m_renderViewport == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Layer2D::createRenderViewport_ %s invalid create RenderViewport"
					, this->getName().c_str()
					);

				return;
			}

			this->addChild( m_renderViewport );
		}

		m_renderCamera->setProxyViewMatrix( true );
		m_renderCamera->setOrthogonalViewport( m_viewport );
        m_renderViewport->setViewport( m_viewport );
				
		Node::setRenderCamera( m_renderCamera );
		Node::setRenderViewport( m_renderViewport );
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
        if( m_renderViewport != nullptr )
        {
		    m_renderViewport->destroy();
		    m_renderViewport = nullptr;
        }

		if( m_renderCamera != nullptr )
		{
			m_renderCamera->destroy();
			m_renderCamera = nullptr;
		}

		Node::setRenderCamera( nullptr );
		Node::setRenderViewport( nullptr );
	}
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::setImageMask( const ResourceImagePtr & _resourceImageMask )
    {
        m_resourceImageMask = _resourceImageMask;

        m_hasImageMask = true;

        this->createRenderTarget_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::removeImageMask()
    {
        m_renderTarget = nullptr;        
        m_resourceImageMask = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::createRenderTarget_()
    {
        RenderTargetInterfacePtr renderTarget = RENDER_SYSTEM( m_serviceProvider )
            ->createRenderTargetTexture( (uint32_t)m_size.x, (uint32_t)m_size.y, PF_A8R8G8B8 );

        Node::setRenderTarget( renderTarget );

        RenderImageInterfacePtr renderTargetImage = RENDER_SYSTEM( m_serviceProvider )
            ->createRenderTargetImage( renderTarget );
        
        RenderTextureInterfacePtr renderTargetTexture = RENDERTEXTURE_SERVICE( m_serviceProvider )
            ->createRenderTexture( renderTargetImage, (uint32_t)m_size.x, (uint32_t)m_size.y );

        const RenderTextureInterfacePtr & renderTargetTextureMask = m_resourceImageMask->getTexture();

        RenderTextureInterfacePtr texures[2];
        texures[0] = renderTargetTexture;
        texures[1] = renderTargetTextureMask;

        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE( m_serviceProvider )
            ->getMaterial( STRINGIZE_STRING_LOCAL( m_serviceProvider, "TrackMatte_Blend" ), PT_TRIANGLELIST, 2, texures );
        
        m_materialImageMask = material;
                
        float hwWidth = (float)renderTarget->getHWWidth();
        float hwHeight = (float)renderTarget->getHWHeight();

        float uv_width = m_size.x / hwWidth;
        float uv_height = m_size.y / hwHeight;

        m_verticesImageMaskWM[0].uv[0] = mt::vec2f( 0.f, 0.f );
        m_verticesImageMaskWM[1].uv[0] = mt::vec2f( uv_width, 0.f );
        m_verticesImageMaskWM[2].uv[0] = mt::vec2f( uv_width, uv_height );
        m_verticesImageMaskWM[3].uv[0] = mt::vec2f( 0.f, uv_height );

        const mt::uv4f & uv_mask = renderTargetTextureMask->getUV();

        m_verticesImageMaskWM[0].uv[1] = uv_mask.p0;
        m_verticesImageMaskWM[1].uv[1] = uv_mask.p1;
        m_verticesImageMaskWM[2].uv[1] = uv_mask.p2;
        m_verticesImageMaskWM[3].uv[1] = uv_mask.p3;
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_renderTarget( RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask )
    {
        ColourValue color;
        this->calcTotalColor( color );

        uint32_t argb = color.getAsARGB();

        const mt::mat4f & wm = this->getWorldMatrix();

        m_verticesImageMaskWM[0].color = argb;
        m_verticesImageMaskWM[1].color = argb;
        m_verticesImageMaskWM[2].color = argb;
        m_verticesImageMaskWM[3].color = argb;

        mt::mul_v3_v2_m4( m_verticesImageMaskWM[0].position, mt::vec2f( 0.f, 0.f), wm );
        mt::mul_v3_v2_m4( m_verticesImageMaskWM[1].position, mt::vec2f( m_size.x, 0.f ), wm );
        mt::mul_v3_v2_m4( m_verticesImageMaskWM[2].position, mt::vec2f( m_size.x, m_size.y ), wm );
        mt::mul_v3_v2_m4( m_verticesImageMaskWM[3].position, mt::vec2f( 0.f, m_size.y ), wm );
                
        const mt::box2f & bb = this->getBoundingBox();

        _renderService
            ->addRenderQuad( _state, m_materialImageMask, m_verticesImageMaskWM, 4, &bb, false );
    }
}
