
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
		, m_invalidateQuads( true )
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

		rebuildWindow_();

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
	void Window::_render( unsigned int _debugMask )
	{
		Node::_render( _debugMask );

		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();

		if( m_invalidateQuads == true )
		{
			rebuildWindow_();
		}

		if( m_invalidateColor == true )
		{
			ColourValue& color = getWorldColor();
			unsigned int argb = color.getAsARGB();

			for( int i = 0; i < MAX_WINDOW_ELEMENTS; ++i )
			{
				ApplyColor2D applyColor( argb );
				std::for_each( m_vertices[i], m_vertices[i]+4, applyColor );
			}
		}

		renderEngine->renderObject2D( m_material[1], &m_textures[1], 1, m_vertices[1], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[3], &m_textures[3], 1, m_vertices[3], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[5], &m_textures[5], 1, m_vertices[5], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[7], &m_textures[7], 1, m_vertices[7], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[2], &m_textures[2], 1, m_vertices[2], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[6], &m_textures[6], 1, m_vertices[6], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[4], &m_textures[4], 1, m_vertices[4], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[8], &m_textures[8], 1, m_vertices[8], 4, LPT_QUAD );
		renderEngine->renderObject2D( m_material[0], &m_textures[0], 1, m_vertices[0], 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::rebuildWindow_()
	{
		const mt::mat3f& wm = getWorldMatrix();

		mt::vec2f transform1X, transform5X, transformClX;
		mt::vec2f transfrom1Y, transform5Y, transfromClY;

		mt::mul_v2_m3_r( transform1X, mt::vec2f( m_initialSizes[1].x, 0.0f ), wm );
		mt::mul_v2_m3_r( transfrom1Y, mt::vec2f( 0.0f, m_initialSizes[1].y ), wm );
		mt::mul_v2_m3_r( transform5X, mt::vec2f( m_initialSizes[5].x, 0.0f ), wm );
		mt::mul_v2_m3_r( transform5Y, mt::vec2f( 0.0f, m_initialSizes[5].y ), wm );
		mt::mul_v2_m3_r( transformClX, mt::vec2f( m_clientSize.x, 0.0f ), wm );
		mt::mul_v2_m3_r( transfromClY, mt::vec2f( 0.0f, m_clientSize.y ), wm );

		//m_quads[1].a = wm.v2.v2;
		m_quads[1].a.x = wm.v2.x;
		m_quads[1].a.y = wm.v2.y;
		m_quads[1].b = m_quads[1].a + transform1X;
		m_quads[1].c = m_quads[1].b + transfrom1Y;
		m_quads[1].d = m_quads[1].a + transfrom1Y;

		m_quads[2].a = m_quads[1].b;
		m_quads[2].b = m_quads[2].a + transformClX;
		m_quads[2].c = m_quads[2].b + transfrom1Y;
		m_quads[2].d = m_quads[1].c;

		m_quads[3].a = m_quads[2].b;
		m_quads[3].b = m_quads[3].a + transform5X;
		m_quads[3].c = m_quads[3].b + transfrom1Y;
		m_quads[3].d = m_quads[2].c;

		m_quads[4].a = m_quads[3].d;
		m_quads[4].b = m_quads[3].c;
		m_quads[4].c = m_quads[4].b + transfromClY;
		m_quads[4].d = m_quads[4].a + transfromClY;

		m_quads[5].a = m_quads[4].d;
		m_quads[5].b = m_quads[4].c;
		m_quads[5].c = m_quads[5].b + transform5Y;
		m_quads[5].d = m_quads[5].a + transform5Y;

		m_quads[6].a = m_quads[5].a - transformClX;
		m_quads[6].b = m_quads[5].a;
		m_quads[6].c = m_quads[5].d;
		m_quads[6].d = m_quads[6].a + transform5Y;

		m_quads[7].a = m_quads[6].a - transform1X;
		m_quads[7].b = m_quads[6].a;
		m_quads[7].c = m_quads[6].d;
		m_quads[7].d = m_quads[7].a + transform5Y;

		m_quads[8].a = m_quads[1].d;
		m_quads[8].b = m_quads[1].c;
		m_quads[8].c = m_quads[7].b;
		m_quads[8].d = m_quads[7].a;

		m_quads[0].a = m_quads[1].c;
		m_quads[0].b = m_quads[3].d;
		m_quads[0].c = m_quads[5].a;
		m_quads[0].d = m_quads[7].b;

		m_UVs[0] = mt::vec2f( m_clientSize.x / m_initialSizes[0].x, m_clientSize.y / m_initialSizes[0].y );
		m_UVs[1] = mt::vec2f( 1.0f, 1.0f );
		m_UVs[2] = mt::vec2f( m_clientSize.x / m_initialSizes[2].x, 1.0f );
		m_UVs[3] = mt::vec2f( 1.0f, 1.0f );
		m_UVs[4] = mt::vec2f( 1.0f, m_clientSize.y / m_initialSizes[4].y );
		m_UVs[5] = mt::vec2f( 1.0f, 1.0f );
		m_UVs[6] = mt::vec2f( m_clientSize.x / m_initialSizes[6].x, 1.0f );
		m_UVs[7] = mt::vec2f( 1.0f, 1.0f );
		m_UVs[8] = mt::vec2f( 1.0f, m_clientSize.y / m_initialSizes[8].x );

		for( int i = 0; i < MAX_WINDOW_ELEMENTS; ++i )
		{
			m_vertices[i][0].pos[0] = m_quads[i].a.x;
			m_vertices[i][0].pos[1] = m_quads[i].a.y;
			m_vertices[i][0].uv[0] = 0.0f;
			m_vertices[i][0].uv[1] = 0.0f;

			m_vertices[i][1].pos[0] = m_quads[i].b.x;
			m_vertices[i][1].pos[1] = m_quads[i].b.y;
			m_vertices[i][1].uv[0] = m_UVs[i].x;
			m_vertices[i][1].uv[1] = 0.0f;

			m_vertices[i][2].pos[0] = m_quads[i].c.x;
			m_vertices[i][2].pos[1] = m_quads[i].c.y;
			m_vertices[i][2].uv[0] = m_UVs[i].x;
			m_vertices[i][2].uv[1] = m_UVs[i].y;

			m_vertices[i][3].pos[0] = m_quads[i].d.x;
			m_vertices[i][3].pos[1] = m_quads[i].d.y;
			m_vertices[i][3].uv[0] = 0.0f;
			m_vertices[i][3].uv[1] = m_UVs[i].y;
		}
		
		m_invalidateQuads = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		//Node::_updateBoundingBox( _boundingBox );
		mt::reset( _boundingBox, m_quads[1].a );
		//mt::add_internal_point( _boundingBox, m_quads[1].a );
		mt::add_internal_point( _boundingBox, m_quads[5].c );
		mt::add_internal_point( _boundingBox, m_quads[3].b );
		mt::add_internal_point( _boundingBox, m_quads[7].d );
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		invalidateQuads();
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::invalidateQuads()
	{
		m_invalidateQuads = true;
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
		invalidateQuads();
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
			MENGE_LOG_ERROR( "Warning: invalid call \"Window::getWindowSize\". Node not compiled. ClientSize will be returned" );
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
			MENGE_LOG_ERROR( "Error: \"Window::getTileSize\" invalid tile argument \"%d\"", _tile );
			return mt::vec2f::zero_v2;
		}
		return m_initialSizes[_tile];
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::setClientSizeInTiles( const mt::vec2f& _tiles )
	{
		if( isCompile() == false )
		{
			MENGE_LOG_ERROR( "Warning: invalid call \"Window::getClientSizeInTiles\". Node not compiled." );
		}
		setClientSize( mt::vec2f( static_cast<int>( _tiles.x ) * m_initialSizes[0].x,
									static_cast<int>( _tiles.y ) * m_initialSizes[0].y ) );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
