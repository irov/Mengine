
#	include "Window.h"
#	include "ObjectImplement.h"

#	include "ResourceManager.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "Material.h"

#	include "Texture.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(Window);
	//////////////////////////////////////////////////////////////////////////
	Window::Window()
		: m_clientSize( 100.0f, 100.0f )
		, m_invalidateVertices( true )
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
	void Window::loader( XmlElement* _xml )
	{
		Node::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "WindowResource", "Name", m_resourceName );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "ClientSize", "Value", &Window::setClientSize );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "ClientSizeInTiles", "Value", &Window::setClientSizeInTiles );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Window::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		m_resource = Holder<ResourceManager>::hostage()
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
			Texture* image = m_resource->getImage( i );
			if( image != NULL )
			{
				m_initialSizes[i].x = image->getWidth();
				m_initialSizes[i].y = image->getHeight();
			}
		}

		for( int i = 0; i < MAX_WINDOW_ELEMENTS; ++i )
		{
			m_material[i] = Holder<RenderEngine>::hostage()
									->createMaterial();

			//m_material[i]->textureStages = 1;
			m_textures[i] = m_resource->getImage( i );
			m_material[i]->blendSrc = BF_SOURCE_ALPHA;
			m_material[i]->blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
			m_material[i]->textureStage[0].colorOp = TOP_MODULATE;
			m_material[i]->textureStage[0].alphaOp = TOP_MODULATE;
		}
		m_material[0]->textureStage[0].addressU = TAM_WRAP;
		m_material[0]->textureStage[0].addressV = TAM_WRAP;
		m_material[1]->textureStage[0].addressU = TAM_CLAMP;
		m_material[1]->textureStage[0].addressV = TAM_CLAMP;
		m_material[2]->textureStage[0].addressU = TAM_WRAP;
		m_material[2]->textureStage[0].addressV = TAM_CLAMP;
		m_material[3]->textureStage[0].addressU = TAM_CLAMP;
		m_material[3]->textureStage[0].addressV = TAM_CLAMP;
		m_material[4]->textureStage[0].addressU = TAM_CLAMP;
		m_material[4]->textureStage[0].addressV = TAM_WRAP;
		m_material[5]->textureStage[0].addressU = TAM_CLAMP;
		m_material[5]->textureStage[0].addressV = TAM_CLAMP;
		m_material[6]->textureStage[0].addressU = TAM_WRAP;
		m_material[6]->textureStage[0].addressV = TAM_CLAMP;
		m_material[7]->textureStage[0].addressU = TAM_CLAMP;
		m_material[7]->textureStage[0].addressV = TAM_CLAMP;
		m_material[8]->textureStage[0].addressU = TAM_CLAMP;
		m_material[8]->textureStage[0].addressV = TAM_WRAP;

		m_invalidateVertices = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_release()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; ++i )
		{
			Holder<RenderEngine>::hostage()
				->releaseMaterial( m_material[i] );
			m_material[i] = NULL;
		}

		if( m_resource != NULL )
		{
			Holder<ResourceManager>::hostage()
				->releaseResource( m_resource );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_render( Camera2D * _camera )
	{
		Node::_render( _camera );

		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();

		Vertex2D * vertices = getVertices();

		renderEngine->renderObject2D( m_material[1], &m_textures[1], 1, &vertices[1*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[3], &m_textures[3], 1, &vertices[3*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[5], &m_textures[5], 1, &vertices[5*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[7], &m_textures[7], 1, &vertices[7*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[2], &m_textures[2], 1, &vertices[2*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[6], &m_textures[6], 1, &vertices[6*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[4], &m_textures[4], 1, &vertices[4*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[8], &m_textures[8], 1, &vertices[8*4], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[0], &m_textures[0], 1, &vertices[0*4], 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::updateVertices()
	{
		m_invalidateVertices = false;

		const mt::mat3f& wm = getWorldMatrix();

		mt::vec2f transform1X, transform5X, transformClX;
		mt::vec2f transfrom1Y, transform5Y, transfromClY;

		mt::mul_v2_m3_r( transform1X, mt::vec2f( m_initialSizes[1].x, 0.0f ), wm );
		mt::mul_v2_m3_r( transfrom1Y, mt::vec2f( 0.0f, m_initialSizes[1].y ), wm );
		mt::mul_v2_m3_r( transform5X, mt::vec2f( m_initialSizes[5].x, 0.0f ), wm );
		mt::mul_v2_m3_r( transform5Y, mt::vec2f( 0.0f, m_initialSizes[5].y ), wm );
		mt::mul_v2_m3_r( transformClX, mt::vec2f( m_clientSize.x, 0.0f ), wm );
		mt::mul_v2_m3_r( transfromClY, mt::vec2f( 0.0f, m_clientSize.y ), wm );

		TQuad quads[MAX_WINDOW_ELEMENTS];

		//quads[1].a = wm.v2.v2;
		quads[1].a.x = wm.v2.x;
		quads[1].a.y = wm.v2.y;
		quads[1].b = quads[1].a + transform1X;
		quads[1].c = quads[1].b + transfrom1Y;
		quads[1].d = quads[1].a + transfrom1Y;

		quads[2].a = quads[1].b;
		quads[2].b = quads[2].a + transformClX;
		quads[2].c = quads[2].b + transfrom1Y;
		quads[2].d = quads[1].c;

		quads[3].a = quads[2].b;
		quads[3].b = quads[3].a + transform5X;
		quads[3].c = quads[3].b + transfrom1Y;
		quads[3].d = quads[2].c;

		quads[4].a = quads[3].d;
		quads[4].b = quads[3].c;
		quads[4].c = quads[4].b + transfromClY;
		quads[4].d = quads[4].a + transfromClY;

		quads[5].a = quads[4].d;
		quads[5].b = quads[4].c;
		quads[5].c = quads[5].b + transform5Y;
		quads[5].d = quads[5].a + transform5Y;

		quads[6].a = quads[5].a - transformClX;
		quads[6].b = quads[5].a;
		quads[6].c = quads[5].d;
		quads[6].d = quads[6].a + transform5Y;

		quads[7].a = quads[6].a - transform1X;
		quads[7].b = quads[6].a;
		quads[7].c = quads[6].d;
		quads[7].d = quads[7].a + transform5Y;

		quads[8].a = quads[1].d;
		quads[8].b = quads[1].c;
		quads[8].c = quads[7].b;
		quads[8].d = quads[7].a;

		quads[0].a = quads[1].c;
		quads[0].b = quads[3].d;
		quads[0].c = quads[5].a;
		quads[0].d = quads[7].b;

		mt::vec2f uvs[MAX_WINDOW_ELEMENTS];

		uvs[0] = mt::vec2f( m_clientSize.x / m_initialSizes[0].x, m_clientSize.y / m_initialSizes[0].y );
		uvs[1] = mt::vec2f( 1.0f, 1.0f );
		uvs[2] = mt::vec2f( m_clientSize.x / m_initialSizes[2].x, 1.0f );
		uvs[3] = mt::vec2f( 1.0f, 1.0f );
		uvs[4] = mt::vec2f( 1.0f, m_clientSize.y / m_initialSizes[4].y );
		uvs[5] = mt::vec2f( 1.0f, 1.0f );
		uvs[6] = mt::vec2f( m_clientSize.x / m_initialSizes[6].x, 1.0f );
		uvs[7] = mt::vec2f( 1.0f, 1.0f );
		uvs[8] = mt::vec2f( 1.0f, m_clientSize.y / m_initialSizes[8].x );

		for( int i = 0; i < MAX_WINDOW_ELEMENTS; ++i )
		{
			m_vertices[i*4 + 0].pos[0] = quads[i].a.x;
			m_vertices[i*4 + 0].pos[1] = quads[i].a.y;
			m_vertices[i*4 + 0].uv[0] = 0.0f;
			m_vertices[i*4 + 0].uv[1] = 0.0f;

			m_vertices[i*4 + 1].pos[0] = quads[i].b.x;
			m_vertices[i*4 + 1].pos[1] = quads[i].b.y;
			m_vertices[i*4 + 1].uv[0] = uvs[i].x;
			m_vertices[i*4 + 1].uv[1] = 0.0f;

			m_vertices[i*4 + 2].pos[0] = quads[i].c.x;
			m_vertices[i*4 + 2].pos[1] = quads[i].c.y;
			m_vertices[i*4 + 2].uv[0] = uvs[i].x;
			m_vertices[i*4 + 2].uv[1] = uvs[i].y;

			m_vertices[i*4 + 3].pos[0] = quads[i].d.x;
			m_vertices[i*4 + 3].pos[1] = quads[i].d.y;
			m_vertices[i*4 + 3].uv[0] = 0.0f;
			m_vertices[i*4 + 3].uv[1] = uvs[i].y;
		}

		const ColourValue& color = getWorldColor();
		unsigned int argb = color.getAsARGB();

		ApplyColor2D applyColor( argb );
		std::for_each( m_vertices, m_vertices + MAX_WINDOW_ELEMENTS*4, applyColor );
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		Vertex2D * vertices = getVertices();

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
		invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::setClientSizeClip( const mt::vec2f& _clientSize )
	{
		mt::vec2f clSize = _clientSize;
		if( m_initialSizes[0].x > 0.001f )
		{
			clSize.x = ::ceilf( clSize.x / m_initialSizes[0].x ) * m_initialSizes[0].x;	
		}
		if( m_initialSizes[0].y > 0.001f )
		{
			clSize.y = ::ceilf( clSize.y / m_initialSizes[0].y ) * m_initialSizes[0].y;
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
			
			return mt::vec2f::zero_v2;
		}

		mt::vec2f windowSize = m_clientSize;
		windowSize += m_initialSizes[1];
		windowSize += m_initialSizes[5];
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

			return mt::vec2f::zero_v2;
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

		setClientSize( mt::vec2f( static_cast<int>( _tiles.x ) * m_initialSizes[0].x,
									static_cast<int>( _tiles.y ) * m_initialSizes[0].y ) );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
