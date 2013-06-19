#	include "Window.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/RenderSystemInterface.h"

#   include "Logger/Logger.h"

#	include "Consts.h"

#   include <algorithm>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    const ETextureAddressMode c_WindowAddressModeU[ResourceWindow_Count] =
    { TAM_WRAP //ResourceWindow_Background
    , TAM_CLAMP //ResourceWindow_LeftTop
    , TAM_WRAP //ResourceWindow_Top
    , TAM_CLAMP //ResourceWindow_RightTop
    , TAM_CLAMP //ResourceWindow_Right
    , TAM_CLAMP //ResourceWindow_RightBottom
    , TAM_WRAP //ResourceWindow_Bottom
    , TAM_CLAMP //ResourceWindow_LeftBottom
    , TAM_CLAMP //ResourceWindow_Left
    };
    //////////////////////////////////////////////////////////////////////////
    const ETextureAddressMode c_WindowAddressModeV[ResourceWindow_Count] =
    { TAM_WRAP //ResourceWindow_Background
    , TAM_CLAMP //ResourceWindow_LeftTop
    , TAM_CLAMP //ResourceWindow_Top
    , TAM_CLAMP //ResourceWindow_RightTop
    , TAM_WRAP //ResourceWindow_Right
    , TAM_CLAMP //ResourceWindow_RightBottom
    , TAM_CLAMP //ResourceWindow_Bottom
    , TAM_CLAMP //ResourceWindow_LeftBottom
    , TAM_WRAP //ResourceWindow_Left
    };
	//////////////////////////////////////////////////////////////////////////
	Window::Window()
		: m_clientSize(100.f, 100.f)
		, m_resource(nullptr)
	{
		for( size_t i = 0; i < ResourceWindow_Count; i++ )
		{
            WindowEdge & edge = m_edge[i];

			edge.initialSize.x = 0.f;
            edge.initialSize.y = 0.f;
            edge.material = nullptr;
            edge.textureCount = 0;
            edge.textures[0] = nullptr;
            edge.textures[1] = nullptr;
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

		m_resource = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceT<ResourceWindow>( m_resourceName );

		if( m_resource == nullptr )
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

            if( image == nullptr )
            {
                continue;
            }

            WindowEdge & edge = m_edge[i];
                        
            const mt::vec2f & size = image->getSize();
            edge.initialSize = size;
           
            const RenderTextureInterfacePtr & texture = image->getTexture();
            const RenderTextureInterfacePtr & textureAlpha = image->getTextureAlpha();

            edge.textures[0] = texture;
            edge.textures[1] = textureAlpha;

            if( textureAlpha == nullptr )
            {
                const RenderMaterialGroup * materialGroup = RENDER_SERVICE(m_serviceProvider)
                    ->getMaterialGroup( CONST_STRING(m_serviceProvider, BlendSprite) );


                ETextureAddressMode U = c_WindowAddressModeU[i];
                ETextureAddressMode V = c_WindowAddressModeV[i];
                edge.material = materialGroup->getMaterial( U, V );

                edge.textureCount = 1;
            }
            else
            {
                const RenderMaterialGroup * materialGroup = RENDER_SERVICE(m_serviceProvider)
                    ->getMaterialGroup( CONST_STRING(m_serviceProvider, ExternalAlpha) );


                ETextureAddressMode U = c_WindowAddressModeU[i];
                ETextureAddressMode V = c_WindowAddressModeV[i];
                edge.material = materialGroup->getMaterial( U, V );

                edge.textureCount = 2;
            }            
		}

		this->invalidateVertices();
		this->invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_release()
	{
		for( size_t i = 0; i != ResourceWindow_Count; ++i )
		{
            WindowEdge & edge = m_edge[i];

			edge.material = nullptr;
            edge.textures[0] = nullptr;
            edge.textures[1] = nullptr;
		}

		if( m_resource != nullptr )
		{
			m_resource->decrementReference();
			m_resource = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_render( RenderCameraInterface * _camera )
	{
		Node::_render( _camera );

		const Vertex2D * vertices = this->getVertices();

        if( this->hasBackground() == true )
        {
            const WindowEdge & edge = m_edge[ResourceWindow_Background];

            RENDER_SERVICE(m_serviceProvider)
                ->addRenderObject2D( _camera, edge.material, edge.textures, edge.textureCount, LPT_QUAD, &vertices[0*4], 4 );
        }

        for( size_t i = 1; i != ResourceWindow_Count; ++i )
        {
            const WindowEdge & edge = m_edge[i];

            RENDER_SERVICE(m_serviceProvider)
                ->addRenderObject2D( _camera, edge.material, edge.textures, edge.textureCount, LPT_QUAD, &vertices[i*4], 4 );
        }
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
		
		quads[ResourceWindow_LeftTop].b.x = quads[ResourceWindow_LeftTop].a.x + m_edge[ResourceWindow_LeftTop].initialSize.x;
		quads[ResourceWindow_LeftTop].b.y = quads[ResourceWindow_LeftTop].a.y;
		
		quads[ResourceWindow_LeftTop].c.x = quads[ResourceWindow_LeftTop].b.x;
		quads[ResourceWindow_LeftTop].c.y = quads[ResourceWindow_LeftTop].b.y + m_edge[ResourceWindow_LeftTop].initialSize.y;

		quads[ResourceWindow_LeftTop].d.x = quads[ResourceWindow_LeftTop].a.x;
		quads[ResourceWindow_LeftTop].d.y = quads[ResourceWindow_LeftTop].c.y;

		/////////////////////////////////////////////////////////////////
		const mt::vec2f & offsetTop = m_resource->getOffset(ResourceWindow_Top);
		
		quads[ResourceWindow_Top].a.x = quads[ResourceWindow_LeftTop].b.x;
		quads[ResourceWindow_Top].a.y = offsetTop.y;
		
		const mt::vec2f & offsetRightTop = m_resource->getOffset(ResourceWindow_RightTop);
		float distance = m_clientSize.x - ( (m_edge[ResourceWindow_LeftTop].initialSize.x + offsetLeftTop.x) + (m_edge[ResourceWindow_RightTop].initialSize.x - offsetRightTop.x) );

		quads[ResourceWindow_Top].b.x = quads[ResourceWindow_Top].a.x + distance;
		quads[ResourceWindow_Top].b.y = quads[ResourceWindow_Top].a.y;

		quads[ResourceWindow_Top].c.x = quads[ResourceWindow_Top].b.x;
		quads[ResourceWindow_Top].c.y = quads[ResourceWindow_Top].a.y + m_edge[ResourceWindow_Top].initialSize.y;

		quads[ResourceWindow_Top].d.x = quads[ResourceWindow_Top].a.x;
		quads[ResourceWindow_Top].d.y = quads[ResourceWindow_Top].c.y;

		/////////////////////////////////////////////////////////////////

		quads[ResourceWindow_RightTop].a.x = quads[ResourceWindow_Top].b.x;
		quads[ResourceWindow_RightTop].a.y = offsetRightTop.y;

		quads[ResourceWindow_RightTop].b.x = quads[ResourceWindow_RightTop].a.x + m_edge[ResourceWindow_RightTop].initialSize.x;
		quads[ResourceWindow_RightTop].b.y = quads[ResourceWindow_RightTop].a.y;

		quads[ResourceWindow_RightTop].c.x = quads[ResourceWindow_RightTop].b.x;
		quads[ResourceWindow_RightTop].c.y = quads[ResourceWindow_RightTop].b.y + m_edge[ResourceWindow_RightTop].initialSize.y;

		quads[ResourceWindow_RightTop].d.x = quads[ResourceWindow_RightTop].a.x;
		quads[ResourceWindow_RightTop].d.y = quads[ResourceWindow_RightTop].c.y;

		/////////////////////////////////////////////////////////////////
		const mt::vec2f & offsetRight = m_resource->getOffset(ResourceWindow_Right);

		quads[ResourceWindow_Right].b.x = m_clientSize.x + offsetRight.x;
		quads[ResourceWindow_Right].b.y = quads[ResourceWindow_RightTop].d.y;
		
		const mt::vec2f & offsetRightBottom = m_resource->getOffset(ResourceWindow_RightBottom);
		float distanceRight = m_clientSize.y - ( (m_edge[ResourceWindow_RightTop].initialSize.y + offsetRightTop.y) + (m_edge[ResourceWindow_RightBottom].initialSize.y - offsetRightBottom.y) );

		quads[ResourceWindow_Right].c.x = quads[ResourceWindow_Right].b.x;
		quads[ResourceWindow_Right].c.y = quads[ResourceWindow_Right].b.y + distanceRight;
		
		quads[ResourceWindow_Right].d.x = quads[ResourceWindow_Right].b.x - m_edge[ResourceWindow_Right].initialSize.x;
		quads[ResourceWindow_Right].d.y = quads[ResourceWindow_Right].c.y;

		quads[ResourceWindow_Right].a.x = quads[ResourceWindow_Right].d.x;
		quads[ResourceWindow_Right].a.y = quads[ResourceWindow_Right].b.y;
			
		/////////////////////////////////////////////////////////////////
		
		quads[ResourceWindow_RightBottom].b.x = m_clientSize.x + offsetRightBottom.x;
		quads[ResourceWindow_RightBottom].b.y = quads[ResourceWindow_Right].c.y;
		
		quads[ResourceWindow_RightBottom].c.x = quads[ResourceWindow_RightBottom].b.x;
		quads[ResourceWindow_RightBottom].c.y = quads[ResourceWindow_RightBottom].b.y + m_edge[ResourceWindow_RightBottom].initialSize.y;
				
		quads[ResourceWindow_RightBottom].d.x = quads[ResourceWindow_RightBottom].c.x - m_edge[ResourceWindow_RightBottom].initialSize.x;
		quads[ResourceWindow_RightBottom].d.y = quads[ResourceWindow_RightBottom].c.y;

		quads[ResourceWindow_RightBottom].a.x = quads[ResourceWindow_RightBottom].d.x;
		quads[ResourceWindow_RightBottom].a.y = quads[ResourceWindow_RightBottom].b.y;
		
		/////////////////////////////////////////////////////////////////
		
		const mt::vec2f & offsetBottom = m_resource->getOffset(ResourceWindow_Bottom);

		quads[ResourceWindow_Bottom].c.x = quads[ResourceWindow_RightBottom].a.x;
		quads[ResourceWindow_Bottom].c.y =  m_clientSize.y + offsetBottom.y;

		const mt::vec2f & offsetLeftBottom = m_resource->getOffset(ResourceWindow_LeftBottom);
		float distanceBottom = m_clientSize.x - ( (m_edge[ResourceWindow_LeftBottom].initialSize.x + offsetLeftBottom.x) + (m_edge[ResourceWindow_RightBottom].initialSize.x - offsetRightBottom.x) );

		quads[ResourceWindow_Bottom].d.x = quads[ResourceWindow_Bottom].c.x - distanceBottom;
		quads[ResourceWindow_Bottom].d.y = quads[ResourceWindow_Bottom].c.y;

		quads[ResourceWindow_Bottom].a.x = quads[ResourceWindow_Bottom].d.x;
		quads[ResourceWindow_Bottom].a.y = quads[ResourceWindow_Bottom].c.y - m_edge[ResourceWindow_Bottom].initialSize.y;

		quads[ResourceWindow_Bottom].b.x = quads[ResourceWindow_Bottom].c.x;
		quads[ResourceWindow_Bottom].b.y = quads[ResourceWindow_Bottom].a.y;
					
		/////////////////////////////////////////////////////////////////
		
		quads[ResourceWindow_LeftBottom].d.x = offsetLeftBottom.x;
		quads[ResourceWindow_LeftBottom].d.y = m_clientSize.y + offsetLeftBottom.y;

		quads[ResourceWindow_LeftBottom].a.x = quads[ResourceWindow_LeftBottom].d.x; 
		quads[ResourceWindow_LeftBottom].a.y = quads[ResourceWindow_LeftBottom].d.y - m_edge[ResourceWindow_LeftBottom].initialSize.y;

		quads[ResourceWindow_LeftBottom].b.x = quads[ResourceWindow_Bottom].a.x;
		quads[ResourceWindow_LeftBottom].b.y = quads[ResourceWindow_LeftBottom].a.y;

		quads[ResourceWindow_LeftBottom].c.x = quads[ResourceWindow_LeftBottom].b.x;
		quads[ResourceWindow_LeftBottom].c.y = quads[ResourceWindow_LeftBottom].d.y;

		/////////////////////////////////////////////////////////////////

		const mt::vec2f & offsetLeft = m_resource->getOffset(ResourceWindow_Left);

		quads[ResourceWindow_Left].d.x =  offsetLeft.x;
		quads[ResourceWindow_Left].d.y = quads[ResourceWindow_LeftBottom].a.y;
					
		float distanceLeft = m_clientSize.y - ( ( m_edge[ResourceWindow_LeftTop].initialSize.y + offsetLeftTop.y ) + (m_edge[ResourceWindow_LeftBottom].initialSize.y - offsetLeftBottom.y) );

		quads[ResourceWindow_Left].a.x = quads[ResourceWindow_Left].d.x;
		quads[ResourceWindow_Left].a.y = quads[ResourceWindow_Left].d.y - distanceLeft;

		quads[ResourceWindow_Left].b.x = quads[ResourceWindow_Left].a.x + m_edge[ResourceWindow_Left].initialSize.x;
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

		uvs[0] = mt::vec2f( m_clientSize.x / m_edge[0].initialSize.x, m_clientSize.y / m_edge[0].initialSize.y );
		uvs[1] = mt::vec2f( 1.f, 1.f );
		uvs[2] = mt::vec2f( m_clientSize.x / m_edge[2].initialSize.x, 1.f );
		uvs[3] = mt::vec2f( 1.f, 1.f );
		uvs[4] = mt::vec2f( 1.f, m_clientSize.y / m_edge[4].initialSize.y );
		uvs[5] = mt::vec2f( 1.f, 1.f );
		uvs[6] = mt::vec2f( m_clientSize.x / m_edge[6].initialSize.x, 1.f );
		uvs[7] = mt::vec2f( 1.f, 1.f );
		uvs[8] = mt::vec2f( 1.f, m_clientSize.y / m_edge[8].initialSize.y );

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
            _vertices[i*4 + 0].uv2.x = uv.x;
            _vertices[i*4 + 0].uv2.y = uv.y;
			
			mt::mul_v2_m4( worldVertice, quad.b, worldMatrix );
			_vertices[i*4 + 1].pos.x = worldVertice.x;
			_vertices[i*4 + 1].pos.y = worldVertice.y;
			_vertices[i*4 + 1].pos.z = 0.f;

			_vertices[i*4 + 1].uv.x = uv.z;
			_vertices[i*4 + 1].uv.y = uv.y;
            _vertices[i*4 + 1].uv2.x = uv.z;
            _vertices[i*4 + 1].uv2.y = uv.y;

			mt::mul_v2_m4( worldVertice, quad.c, worldMatrix );
			_vertices[i*4 + 2].pos.x = worldVertice.x;
			_vertices[i*4 + 2].pos.y = worldVertice.y;
			_vertices[i*4 + 2].pos.z = 0.f;

			_vertices[i*4 + 2].uv.x = uv.z;
			_vertices[i*4 + 2].uv.y = uv.w;
            _vertices[i*4 + 2].uv2.x = uv.z;
            _vertices[i*4 + 2].uv2.y = uv.w;
			
			mt::mul_v2_m4( worldVertice, quad.d, worldMatrix );
			_vertices[i*4 + 3].pos.x = worldVertice.x;
			_vertices[i*4 + 3].pos.y = worldVertice.y;
			_vertices[i*4 + 3].pos.z = 0.f;

			_vertices[i*4 + 3].uv.x = uv.x;
			_vertices[i*4 + 3].uv.y = uv.w;
            _vertices[i*4 + 3].uv2.x = uv.x;
            _vertices[i*4 + 3].uv2.y = uv.w;
		}

		ColourValue color;
		this->calcTotalColor(color);

		unsigned int argb = color.getAsARGB();

		ApplyColor2D applyColor( argb );
		std::for_each( _vertices, _vertices + ResourceWindow_Count * 4, applyColor );
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

		if( m_edge[ResourceWindow_Background].initialSize.x > 0.001f )
		{
			clSize.x = ::ceilf( clSize.x / m_edge[ResourceWindow_Background].initialSize.x ) * m_edge[ResourceWindow_Background].initialSize.x;	
		}

		if( m_edge[ResourceWindow_Background].initialSize.y > 0.001f )
		{
			clSize.y = ::ceilf( clSize.y / m_edge[ResourceWindow_Background].initialSize.y ) * m_edge[ResourceWindow_Background].initialSize.y;
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
		windowSize += m_edge[ResourceWindow_LeftTop].initialSize;
		windowSize += m_edge[ResourceWindow_RightBottom].initialSize;

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

        const mt::vec2f& size = m_edge[_tile].initialSize;

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

		this->setClientSize( mt::vec2f( static_cast<int>( _tiles.x ) * m_edge[ResourceWindow_Background].initialSize.x,
									static_cast<int>( _tiles.y ) * m_edge[ResourceWindow_Background].initialSize.y ) );
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
		if( m_edge[ResourceWindow_Background].textures[0] == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
