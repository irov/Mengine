#	include "Window.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/RenderSystemInterface.h"

#   include "Logger/Logger.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Window::Window()
		: m_clientSize(100.f, 100.f)
		, m_resource(NULL)
	{
		for( int i = 0; i < ResourceWindow_Count; i++ )
		{
			m_initialSizes[i].x = 0.0f;
			m_initialSizes[i].y = 0.0f;
			m_material[i] = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Window::~Window()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Window::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_resource = RESOURCE_SERVICE(m_serviceProvider)->getResourceT<ResourceWindow>( m_resourceName );

		if( m_resource == NULL )
		{
			LOGGER_ERROR(m_serviceProvider)( "Window::_compile %s Error while compiling resources. Resource '%s' does not exist" 
				, this->getName().c_str()
				, m_resourceName.c_str() 
				);

			return false;
		}

		for( int i = 0; i < ResourceWindow_Count; i++ )
		{
			ResourceImage * image = m_resource->getResource( i );

			if( image != NULL )
			{
				m_initialSizes[i] = image->getSize();
			}
		}

		const RenderMaterialGroup * mg_sprite = RENDER_SERVICE(m_serviceProvider)->getMaterialGroup( CONST_STRING(m_serviceProvider, BlendSprite) );

		for( int i = 0; i < ResourceWindow_Count; ++i )
		{
            ResourceImage * image = m_resource->getResource( i );

            if( image != NULL )
            {
                const RenderTextureInterface * texture = image->getTexture();

                m_textures[i] = texture;
            }
            else
            {
                m_textures[i] = NULL;
            }			
		}

		m_material[ResourceWindow_Background] = mg_sprite->getMaterial( TAM_WRAP, TAM_WRAP );
		m_material[ResourceWindow_LeftTop] = mg_sprite->getMaterial( TAM_CLAMP, TAM_CLAMP );
		m_material[ResourceWindow_Top] = mg_sprite->getMaterial( TAM_WRAP, TAM_CLAMP );
		m_material[ResourceWindow_RightTop] = mg_sprite->getMaterial( TAM_CLAMP, TAM_CLAMP );
		m_material[ResourceWindow_Right] = mg_sprite->getMaterial( TAM_CLAMP, TAM_WRAP );
		m_material[ResourceWindow_RightBottom] = mg_sprite->getMaterial( TAM_CLAMP, TAM_CLAMP );
		m_material[ResourceWindow_Bottom] = mg_sprite->getMaterial( TAM_WRAP, TAM_CLAMP );
		m_material[ResourceWindow_LeftBottom] = mg_sprite->getMaterial( TAM_CLAMP, TAM_CLAMP );
		m_material[ResourceWindow_Left] = mg_sprite->getMaterial( TAM_CLAMP, TAM_WRAP );

		this->invalidateVertices();
		this->invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_release()
	{
		for( int i = 0; i < ResourceWindow_Count; ++i )
		{
			m_material[i] = NULL;
		}

		if( m_resource != NULL )
		{
			m_resource->decrementReference();
			m_resource = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_render( RenderCameraInterface * _camera )
	{
		Node::_render( _camera );

		const Vertex2D * vertices = this->getVertices();

        if( this->hasBackground() == true )
        {
            RENDER_SERVICE(m_serviceProvider)->addRenderObject2D( _camera, m_material[ResourceWindow_Background], &m_textures[ResourceWindow_Background], 1, &vertices[0*4], 4, LPT_QUAD );
        }

		RENDER_SERVICE(m_serviceProvider)->addRenderObject2D( _camera, m_material[ResourceWindow_LeftTop], &m_textures[ResourceWindow_LeftTop], 1, &vertices[1*4], 4, LPT_QUAD );
		RENDER_SERVICE(m_serviceProvider)->addRenderObject2D( _camera, m_material[ResourceWindow_RightTop], &m_textures[ResourceWindow_RightTop], 1, &vertices[3*4], 4, LPT_QUAD );
		RENDER_SERVICE(m_serviceProvider)->addRenderObject2D( _camera, m_material[ResourceWindow_RightBottom], &m_textures[ResourceWindow_RightBottom], 1, &vertices[5*4], 4, LPT_QUAD );
		RENDER_SERVICE(m_serviceProvider)->addRenderObject2D( _camera, m_material[ResourceWindow_LeftBottom], &m_textures[ResourceWindow_LeftBottom], 1, &vertices[7*4], 4, LPT_QUAD );
		RENDER_SERVICE(m_serviceProvider)->addRenderObject2D( _camera, m_material[ResourceWindow_Top], &m_textures[ResourceWindow_Top], 1, &vertices[2*4], 4, LPT_QUAD );
		RENDER_SERVICE(m_serviceProvider)->addRenderObject2D( _camera, m_material[ResourceWindow_Bottom], &m_textures[ResourceWindow_Bottom], 1, &vertices[6*4], 4, LPT_QUAD );
		RENDER_SERVICE(m_serviceProvider)->addRenderObject2D( _camera, m_material[ResourceWindow_Right], &m_textures[ResourceWindow_Right], 1, &vertices[4*4], 4, LPT_QUAD );
		RENDER_SERVICE(m_serviceProvider)->addRenderObject2D( _camera, m_material[ResourceWindow_Left], &m_textures[ResourceWindow_Left], 1, &vertices[8*4], 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices )
	{
        (void)_invalidateVertices;

		struct TQuad
		{
			mt::vec2f a,b,c,d;
		};

		TQuad quads[ResourceWindow_Count];

		const mt::vec2f & offsetLeftTop = m_resource->getOffset(ResourceWindow_LeftTop);

		quads[ResourceWindow_LeftTop].a.x = offsetLeftTop.x;
		quads[ResourceWindow_LeftTop].a.y = offsetLeftTop.y;
		
		quads[ResourceWindow_LeftTop].b.x = quads[ResourceWindow_LeftTop].a.x + m_initialSizes[ResourceWindow_LeftTop].x;
		quads[ResourceWindow_LeftTop].b.y = quads[ResourceWindow_LeftTop].a.y;
		
		quads[ResourceWindow_LeftTop].c.x = quads[ResourceWindow_LeftTop].b.x;
		quads[ResourceWindow_LeftTop].c.y = quads[ResourceWindow_LeftTop].b.y + m_initialSizes[ResourceWindow_LeftTop].y;

		quads[ResourceWindow_LeftTop].d.x = quads[ResourceWindow_LeftTop].a.x;
		quads[ResourceWindow_LeftTop].d.y = quads[ResourceWindow_LeftTop].c.y;

		/////////////////////////////////////////////////////////////////
		const mt::vec2f & offsetTop = m_resource->getOffset(ResourceWindow_Top);
		
		quads[ResourceWindow_Top].a.x = quads[ResourceWindow_LeftTop].b.x;
		quads[ResourceWindow_Top].a.y = offsetTop.y;
		
		const mt::vec2f & offsetRightTop = m_resource->getOffset(ResourceWindow_RightTop);
		float distance = m_clientSize.x - ( (m_initialSizes[ResourceWindow_LeftTop].x + offsetLeftTop.x) + (m_initialSizes[ResourceWindow_RightTop].x - offsetRightTop.x) );

		quads[ResourceWindow_Top].b.x = quads[ResourceWindow_Top].a.x + distance;
		quads[ResourceWindow_Top].b.y = quads[ResourceWindow_Top].a.y;

		quads[ResourceWindow_Top].c.x = quads[ResourceWindow_Top].b.x;
		quads[ResourceWindow_Top].c.y = quads[ResourceWindow_Top].a.y + m_initialSizes[ResourceWindow_Top].y;

		quads[ResourceWindow_Top].d.x = quads[ResourceWindow_Top].a.x;
		quads[ResourceWindow_Top].d.y = quads[ResourceWindow_Top].c.y;

		/////////////////////////////////////////////////////////////////

		quads[ResourceWindow_RightTop].a.x = quads[ResourceWindow_Top].b.x;
		quads[ResourceWindow_RightTop].a.y = offsetRightTop.y;

		quads[ResourceWindow_RightTop].b.x = quads[ResourceWindow_RightTop].a.x + m_initialSizes[ResourceWindow_RightTop].x;
		quads[ResourceWindow_RightTop].b.y = quads[ResourceWindow_RightTop].a.y;

		quads[ResourceWindow_RightTop].c.x = quads[ResourceWindow_RightTop].b.x;
		quads[ResourceWindow_RightTop].c.y = quads[ResourceWindow_RightTop].b.y + m_initialSizes[ResourceWindow_RightTop].y;

		quads[ResourceWindow_RightTop].d.x = quads[ResourceWindow_RightTop].a.x;
		quads[ResourceWindow_RightTop].d.y = quads[ResourceWindow_RightTop].c.y;

		/////////////////////////////////////////////////////////////////
		const mt::vec2f & offsetRight = m_resource->getOffset(ResourceWindow_Right);

		quads[ResourceWindow_Right].b.x = m_clientSize.x + offsetRight.x;
		quads[ResourceWindow_Right].b.y = quads[ResourceWindow_RightTop].d.y;
		
		const mt::vec2f & offsetRightBottom = m_resource->getOffset(ResourceWindow_RightBottom);
		float distanceRight = m_clientSize.y - ( (m_initialSizes[ResourceWindow_RightTop].y + offsetRightTop.y) + (m_initialSizes[ResourceWindow_RightBottom].y - offsetRightBottom.y) );

		quads[ResourceWindow_Right].c.x = quads[ResourceWindow_Right].b.x;
		quads[ResourceWindow_Right].c.y = quads[ResourceWindow_Right].b.y + distanceRight;
		
		quads[ResourceWindow_Right].d.x = quads[ResourceWindow_Right].b.x - m_initialSizes[ResourceWindow_Right].x;
		quads[ResourceWindow_Right].d.y = quads[ResourceWindow_Right].c.y;

		quads[ResourceWindow_Right].a.x = quads[ResourceWindow_Right].d.x;
		quads[ResourceWindow_Right].a.y = quads[ResourceWindow_Right].b.y;
			
		/////////////////////////////////////////////////////////////////
		
		quads[ResourceWindow_RightBottom].b.x = m_clientSize.x + offsetRightBottom.x;
		quads[ResourceWindow_RightBottom].b.y = quads[ResourceWindow_Right].c.y;
		
		quads[ResourceWindow_RightBottom].c.x = quads[ResourceWindow_RightBottom].b.x;
		quads[ResourceWindow_RightBottom].c.y = quads[ResourceWindow_RightBottom].b.y + m_initialSizes[ResourceWindow_RightBottom].y;
				
		quads[ResourceWindow_RightBottom].d.x = quads[ResourceWindow_RightBottom].c.x - m_initialSizes[ResourceWindow_RightBottom].x;
		quads[ResourceWindow_RightBottom].d.y = quads[ResourceWindow_RightBottom].c.y;

		quads[ResourceWindow_RightBottom].a.x = quads[ResourceWindow_RightBottom].d.x;
		quads[ResourceWindow_RightBottom].a.y = quads[ResourceWindow_RightBottom].b.y;
		
		/////////////////////////////////////////////////////////////////
		
		const mt::vec2f & offsetBottom = m_resource->getOffset(ResourceWindow_Bottom);

		quads[ResourceWindow_Bottom].c.x = quads[ResourceWindow_RightBottom].a.x;
		quads[ResourceWindow_Bottom].c.y =  m_clientSize.y + offsetBottom.y;

		const mt::vec2f & offsetLeftBottom = m_resource->getOffset(ResourceWindow_LeftBottom);
		float distanceBottom = m_clientSize.x - ( (m_initialSizes[ResourceWindow_LeftBottom].x + offsetLeftBottom.x) + (m_initialSizes[ResourceWindow_RightBottom].x - offsetRightBottom.x) );

		quads[ResourceWindow_Bottom].d.x = quads[ResourceWindow_Bottom].c.x - distanceBottom;
		quads[ResourceWindow_Bottom].d.y = quads[ResourceWindow_Bottom].c.y;

		quads[ResourceWindow_Bottom].a.x = quads[ResourceWindow_Bottom].d.x;
		quads[ResourceWindow_Bottom].a.y = quads[ResourceWindow_Bottom].c.y - m_initialSizes[ResourceWindow_Bottom].y;

		quads[ResourceWindow_Bottom].b.x = quads[ResourceWindow_Bottom].c.x;
		quads[ResourceWindow_Bottom].b.y = quads[ResourceWindow_Bottom].a.y;
					
		/////////////////////////////////////////////////////////////////
		
		quads[ResourceWindow_LeftBottom].d.x = offsetLeftBottom.x;
		quads[ResourceWindow_LeftBottom].d.y = m_clientSize.y + offsetLeftBottom.y;

		quads[ResourceWindow_LeftBottom].a.x = quads[ResourceWindow_LeftBottom].d.x; 
		quads[ResourceWindow_LeftBottom].a.y = quads[ResourceWindow_LeftBottom].d.y - m_initialSizes[ResourceWindow_LeftBottom].y;

		quads[ResourceWindow_LeftBottom].b.x = quads[ResourceWindow_Bottom].a.x;
		quads[ResourceWindow_LeftBottom].b.y = quads[ResourceWindow_LeftBottom].a.y;

		quads[ResourceWindow_LeftBottom].c.x = quads[ResourceWindow_LeftBottom].b.x;
		quads[ResourceWindow_LeftBottom].c.y = quads[ResourceWindow_LeftBottom].d.y;

		/////////////////////////////////////////////////////////////////

		const mt::vec2f & offsetLeft = m_resource->getOffset(ResourceWindow_Left);

		quads[ResourceWindow_Left].d.x =  offsetLeft.x;
		quads[ResourceWindow_Left].d.y = quads[ResourceWindow_LeftBottom].a.y;
					
		float distanceLeft = m_clientSize.y - ( ( m_initialSizes[ResourceWindow_LeftTop].y + offsetLeftTop.y ) + (m_initialSizes[ResourceWindow_LeftBottom].y - offsetLeftBottom.y) );

		quads[ResourceWindow_Left].a.x = quads[ResourceWindow_Left].d.x;
		quads[ResourceWindow_Left].a.y = quads[ResourceWindow_Left].d.y - distanceLeft;

		quads[ResourceWindow_Left].b.x = quads[ResourceWindow_Left].a.x + m_initialSizes[ResourceWindow_Left].x;
		quads[ResourceWindow_Left].b.y = quads[ResourceWindow_Left].a.y;
		
		quads[ResourceWindow_Left].c.x = quads[ResourceWindow_Left].b.x;
		quads[ResourceWindow_Left].c.y = quads[ResourceWindow_Left].d.y;
		
		/////////////////////////////////////////////////////////////////

		quads[ResourceWindow_Background].a = mt::vec2f( 0.0f, 0.0f );
		quads[ResourceWindow_Background].b = mt::vec2f( m_clientSize.x, 0.0f );
		quads[ResourceWindow_Background].c = mt::vec2f( m_clientSize.x, m_clientSize.y );
		quads[ResourceWindow_Background].d = mt::vec2f( 0.0f, m_clientSize.y );

		/////////////////////////////////////////////////////////////////

		mt::vec2f uvs[ResourceWindow_Count];

		uvs[0] = mt::vec2f( m_clientSize.x / m_initialSizes[0].x, m_clientSize.y / m_initialSizes[0].y );
		uvs[1] = mt::vec2f( 1.0f, 1.0f );
		uvs[2] = mt::vec2f( m_clientSize.x / m_initialSizes[2].x, 1.0f );
		uvs[3] = mt::vec2f( 1.0f, 1.0f );
		uvs[4] = mt::vec2f( 1.0f, m_clientSize.y / m_initialSizes[4].y );
		uvs[5] = mt::vec2f( 1.0f, 1.0f );
		uvs[6] = mt::vec2f( m_clientSize.x / m_initialSizes[6].x, 1.0f );
		uvs[7] = mt::vec2f( 1.0f, 1.0f );
		uvs[8] = mt::vec2f( 1.0f, m_clientSize.y / m_initialSizes[8].y );

		const mt::mat4f& worldMatrix = this->getWorldMatrix();
		mt::vec2f worldVertice;

		for( int i = 0; i < ResourceWindow_Count; ++i )
		{   
            mt::vec4f uv;
            
            uv.x = 0.f;
            uv.y = 0.f;
            uv.z = 1.f;
            uv.w = 1.f;

            ResourceImage * image = m_resource->getResource( i );

            if( image != NULL )
            {
                const mt::vec4f & texture_uv = image->getUVImage();

                float w = texture_uv.z - texture_uv.x;
                float h = texture_uv.w - texture_uv.y;

                uv.x = texture_uv.x;
                uv.y = texture_uv.y;
                uv.z = texture_uv.x + uvs[i].x * w;
                uv.w = texture_uv.y + uvs[i].y * h;
            }

            const TQuad & quad = quads[i];

			mt::mul_v2_m4( worldVertice, quad.a, worldMatrix );
			_vertices[i*4 + 0].pos.x = worldVertice.x;
			_vertices[i*4 + 0].pos.y = worldVertice.y;
			_vertices[i*4 + 0].pos.z = 0.f;
			
			_vertices[i*4 + 0].uv.x = uv.x;
			_vertices[i*4 + 0].uv.y = uv.y;
            _vertices[i*4 + 0].uv2.x = 0.f;
            _vertices[i*4 + 0].uv2.y = 0.f;
			
			mt::mul_v2_m4( worldVertice, quad.b, worldMatrix );
			_vertices[i*4 + 1].pos.x = worldVertice.x;
			_vertices[i*4 + 1].pos.y = worldVertice.y;
			_vertices[i*4 + 1].pos.z = 0.f;

			_vertices[i*4 + 1].uv.x = uv.z;
			_vertices[i*4 + 1].uv.y = uv.y;
            _vertices[i*4 + 1].uv2.x = 0.f;
            _vertices[i*4 + 1].uv2.y = 0.f;

			mt::mul_v2_m4( worldVertice, quad.c, worldMatrix );
			_vertices[i*4 + 2].pos.x = worldVertice.x;
			_vertices[i*4 + 2].pos.y = worldVertice.y;
			_vertices[i*4 + 2].pos.z = 0.f;

			_vertices[i*4 + 2].uv.x = uv.z;
			_vertices[i*4 + 2].uv.y = uv.w;
            _vertices[i*4 + 2].uv2.x = 0.f;
            _vertices[i*4 + 2].uv2.y = 0.f;
			
			mt::mul_v2_m4( worldVertice, quad.d, worldMatrix );
			_vertices[i*4 + 3].pos.x = worldVertice.x;
			_vertices[i*4 + 3].pos.y = worldVertice.y;
			_vertices[i*4 + 3].pos.z = 0.f;

			_vertices[i*4 + 3].uv.x = uv.x;
			_vertices[i*4 + 3].uv.y = uv.w;
            _vertices[i*4 + 3].uv2.x = 0.f;
            _vertices[i*4 + 3].uv2.y = 0.f;
		}

		ColourValue color;
		this->calcTotalColor(color);

		unsigned int argb = color.getAsARGB();

		ApplyColor2D applyColor( argb );
		std::for_each( _vertices, _vertices + ResourceWindow_Count*4, applyColor );
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		const Vertex2D * vertices = this->getVertices();

		mt::reset( _boundingBox, vertices[0].pos[0], vertices[0].pos[1] );

		for( int i = 1; i != 4; ++i )
		{
			mt::add_internal_point( _boundingBox, vertices[i].pos[0], vertices[i].pos[1] );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_invalidateColor()
	{
		Node::_invalidateColor();
		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::setClientSizeClip( const mt::vec2f& _clientSize )
	{
		if( this->hasBackground() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Window::setClientSizeClip: '%s' background not exist"
                , this->getName().c_str()
                );

			return;
		}

		mt::vec2f clSize = _clientSize;

		if( m_initialSizes[ResourceWindow_Background].x > 0.001f )
		{
			clSize.x = ::ceilf( clSize.x / m_initialSizes[ResourceWindow_Background].x ) * m_initialSizes[ResourceWindow_Background].x;	
		}

		if( m_initialSizes[ResourceWindow_Background].y > 0.001f )
		{
			clSize.y = ::ceilf( clSize.y / m_initialSizes[ResourceWindow_Background].y ) * m_initialSizes[ResourceWindow_Background].y;
		}
		
		this->setClientSize( clSize );
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::setClientSize( const mt::vec2f& _clientSize )
	{
		m_clientSize = _clientSize;

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f Window::getClientSize() const
	{
		return m_clientSize;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Window::getWindowSize() const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Window::getWindowSize: '%s' not compiled"
                , this->getName().c_str()
                );
			
			return mt::zero_v2;
		}

		mt::vec2f windowSize = m_clientSize;
		windowSize += m_initialSizes[ResourceWindow_LeftTop];
		windowSize += m_initialSizes[ResourceWindow_RightBottom];

		return windowSize;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Window::getTileSize( int _tile ) const
	{
		if( _tile < 0 || _tile > ResourceWindow_Count )
		{
			LOGGER_ERROR(m_serviceProvider)( "Window::getTileSize: %s invalid tile argument '%d'"
                , this->getName().c_str()
				, _tile 
				);

			return mt::zero_v2;
		}

        const mt::vec2f& size = m_initialSizes[_tile];

		return size;
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::setClientSizeInTiles( const mt::vec2f& _tiles )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Window::setClientSizeInTiles: '%s' not compiled."
                , this->getName().c_str()
                );

			return;
		}

		if( this->hasBackground() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Window::setClientSizeInTiles: '%s' background not exist"
                , this->getName().c_str()
                );

			return;
		}

		this->setClientSize( mt::vec2f( static_cast<int>( _tiles.x ) * m_initialSizes[ResourceWindow_Background].x,
									static_cast<int>( _tiles.y ) * m_initialSizes[ResourceWindow_Background].y ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::setResourceWindow( const ConstString & _resourceName )
	{
		if( m_resourceName == _resourceName )
		{
			return;
		}

		m_resourceName = _resourceName;

        this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Window::hasBackground() const
	{
		if( m_textures[ResourceWindow_Background] == NULL )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
