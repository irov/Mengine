
#	include "Window.h"

#	include "ResourceManager.h"

#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "RenderMaterial.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Window::Window()
		: m_clientSize( 100.0f, 100.0f )
		, m_resource( NULL )
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
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

		m_resource = ResourceManager::get()
			->getResourceT<ResourceWindow>( m_resourceName );

		if( m_resource == NULL )
		{
			MENGE_LOG_ERROR( "Window: Error while compiling resources. Resource '%s' does not exist" 
				, m_resourceName.c_str() 
				);

			return false;
		}

		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			RenderTextureInterface* image = m_resource->getImage( i );
			if( image != NULL )
			{
				m_initialSizes[i].x = image->getWidth();
				m_initialSizes[i].y = image->getHeight();
			}
		}

		const RenderMaterialGroup * mg_sprite = RenderEngine::get()
			->getMaterialGroup( CONST_STRING(BlendSprite) );

		for( int i = 0; i < MAX_WINDOW_ELEMENTS; ++i )
		{
			m_textures[i] = m_resource->getImage( i );
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

		invalidateVertices();
		invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_release()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; ++i )
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

		RenderEngine* renderEngine = RenderEngine::get();

		const Vertex2D * vertices = this->getVertices();

		renderEngine->renderObject2D( _camera, m_material[ResourceWindow_LeftTop], &m_textures[ResourceWindow_LeftTop], NULL, 1, &vertices[1*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( _camera, m_material[ResourceWindow_RightTop], &m_textures[ResourceWindow_RightTop], NULL, 1, &vertices[3*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( _camera, m_material[ResourceWindow_RightBottom], &m_textures[ResourceWindow_RightBottom], NULL, 1, &vertices[5*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( _camera, m_material[ResourceWindow_LeftBottom], &m_textures[ResourceWindow_LeftBottom], NULL, 1, &vertices[7*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( _camera, m_material[ResourceWindow_Top], &m_textures[ResourceWindow_Top], NULL, 1, &vertices[2*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( _camera, m_material[ResourceWindow_Bottom], &m_textures[ResourceWindow_Bottom], NULL, 1, &vertices[6*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( _camera, m_material[ResourceWindow_Right], &m_textures[ResourceWindow_Right], NULL, 1, &vertices[4*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( _camera, m_material[ResourceWindow_Left], &m_textures[ResourceWindow_Left], NULL, 1, &vertices[8*4], 4, LPT_QUAD );
		
		if( this->hasBackground() == true )
		{
			renderEngine->renderObject2D( _camera, m_material[ResourceWindow_Background], &m_textures[ResourceWindow_Background], NULL, 1, &vertices[0*4], 4, LPT_QUAD );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_updateVertices( Vertex2D * _vertices, unsigned char _invalidateVertices )
	{
		struct TQuad
		{
			mt::vec2f a,b,c,d;
		};

		TQuad quads[MAX_WINDOW_ELEMENTS];

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

		mt::vec2f uvs[MAX_WINDOW_ELEMENTS];

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

		for( int i = 0; i < MAX_WINDOW_ELEMENTS; ++i )
		{
			mt::mul_v2_m4(worldVertice,quads[i].a,worldMatrix);
			//_vertices[i*4 + 0].pos[0] = quads[i].a.x;
			//_vertices[i*4 + 0].pos[1] = quads[i].a.y;
			_vertices[i*4 + 0].pos[0] = worldVertice.x;
			_vertices[i*4 + 0].pos[1] = worldVertice.y;
			_vertices[i*4 + 0].pos[2] = 0.f;
			//_vertices[i*4 + 0].pos[3] = 1.f;
			
			//printf( "%f,%f\n",_vertices[i*4 + 0].pos[0],_vertices[i*4 + 0].pos[1],_vertices[i*4 + 0].pos[1] );
			_vertices[i*4 + 0].uv[0] = 0.0f;
			_vertices[i*4 + 0].uv[1] = 0.0f;
			//printf( "%f,%f\n",_vertices[i*4 + 0].uv[0],_vertices[i*4 + 0].uv[1] );
			
			mt::mul_v2_m4(worldVertice,quads[i].b,worldMatrix);
			//_vertices[i*4 + 1].pos[0] = quads[i].b.x;
			//_vertices[i*4 + 1].pos[1] = quads[i].b.y;
			_vertices[i*4 + 1].pos[0] = worldVertice.x;
			_vertices[i*4 + 1].pos[1] = worldVertice.y;
			_vertices[i*4 + 1].pos[2] = 0.f;
			//_vertices[i*4 + 1].pos[3] = 1.f;

			_vertices[i*4 + 1].uv[0] = uvs[i].x;
			_vertices[i*4 + 1].uv[1] = 0.0f;
			//printf( "%f,%f\n",_vertices[i*4 + 1].uv[0],_vertices[i*4 + 1].uv[1] );

			mt::mul_v2_m4(worldVertice,quads[i].c,worldMatrix);
			//_vertices[i*4 + 2].pos[0] = quads[i].c.x;
			//_vertices[i*4 + 2].pos[1] = quads[i].c.y;
			_vertices[i*4 + 2].pos[0] = worldVertice.x;
			_vertices[i*4 + 2].pos[1] = worldVertice.y;
			_vertices[i*4 + 2].pos[2] = 0.f;
			//_vertices[i*4 + 2].pos[3] = 1.f;

			_vertices[i*4 + 2].uv[0] = uvs[i].x;
			_vertices[i*4 + 2].uv[1] = uvs[i].y;
			//printf( "%f,%f\n",_vertices[i*4 + 2].uv[0],_vertices[i*4 + 2].uv[1] );
			
			mt::mul_v2_m4(worldVertice,quads[i].d,worldMatrix);
			//_vertices[i*4 + 3].pos[0] = quads[i].d.x;
			//_vertices[i*4 + 3].pos[1] = quads[i].d.y;
			_vertices[i*4 + 3].pos[0] = worldVertice.x;
			_vertices[i*4 + 3].pos[1] = worldVertice.y;
			_vertices[i*4 + 3].pos[2] = 0.f;
			//_vertices[i*4 + 3].pos[3] = 1.f;

			_vertices[i*4 + 3].uv[0] = 0.0f;
			_vertices[i*4 + 3].uv[1] = uvs[i].y;
			//printf( "%f,%f\n",_vertices[i*4 + 3].uv[0],_vertices[i*4 + 3].uv[1] );
			//printf("%s\n","------------------------------------");
		}

		ColourValue color;
		this->calcTotalColor(color);

		unsigned int argb = color.getAsARGB();

		ApplyColor2D applyColor( argb );
		std::for_each( _vertices, _vertices + MAX_WINDOW_ELEMENTS*4, applyColor );
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
		invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_invalidateColor()
	{
		Node::_invalidateColor();
		invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::setClientSizeClip( const mt::vec2f& _clientSize )
	{
		if( this->hasBackground() == false )
		{
			MENGE_LOG_ERROR( "Warning: invalid call 'Window::setClientSizeClip'. background not exist" );
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
		setClientSize( clSize );
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::setClientSize( const mt::vec2f& _clientSize )
	{
		m_clientSize = _clientSize;
		invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f Window::getClientSize() const
	{
		return m_clientSize;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Window::getWindowSize() const
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Warning: invalid call 'Window::getWindowSize'. Node not compiled. ClientSize will be returned" );
			
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
		if( _tile < 0 || _tile > MAX_WINDOW_ELEMENTS )
		{
			MENGE_LOG_ERROR( "Error: 'Window::getTileSize' invalid tile argument '%d'"
				, _tile 
				);

			return mt::zero_v2;
		}
		return m_initialSizes[_tile];
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::setClientSizeInTiles( const mt::vec2f& _tiles )
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Warning: invalid call 'Window::getClientSizeInTiles'. Node not compiled." );
			return;
		}

		if( this->hasBackground() == false )
		{
			MENGE_LOG_ERROR( "Warning: invalid call 'Window::getClientSizeInTiles'. background not exist" );
			return;
		}

		setClientSize( mt::vec2f( static_cast<int>( _tiles.x ) * m_initialSizes[ResourceWindow_Background].x,
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
		_release();
		_compile();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Window::hasBackground()
	{
		if( m_textures[ResourceWindow_Background] == NULL )
		{
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
