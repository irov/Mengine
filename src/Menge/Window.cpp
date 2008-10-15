
#	include "Window.h"
#	include "ObjectImplement.h"

#	include "ResourceManager.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Window );
	//////////////////////////////////////////////////////////////////////////
	Window::Window()
		: m_clientSize( 100.0f, 100.0f )
		, m_invalidateQuads( true )
	{
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
			XML_CASE_ATTRIBUTE_NODE( "ClientSize", "Value", m_clientSize );
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
			MENGE_LOG_ERROR( "Window: Error while compiling resources. Resource \"%s\" does not exist" 
				, m_resourceName.c_str() );
			return false;
		}

		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			RenderImageInterface* image = m_resource->getImage( i );
			if( image != NULL )
			{
				m_initialSizes[i].x = image->getWidth();
				m_initialSizes[i].y = image->getHeight();
			}
			else
			{
				m_initialSizes[i].x = 10.0f;
				m_initialSizes[i].y = 10.0f;
			}
		}

		rebuildWindow_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_release()
	{
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

		renderEngine->renderTriple( m_quads[0].a, m_quads[0].b, m_quads[0].c, m_backUV[0], m_backUV[1], m_backUV[2],
			0xFFFFFFFF, m_resource->getImage(0) );
		renderEngine->renderTriple( m_quads[0].a, m_quads[0].c, m_quads[0].d, m_backUV[0], m_backUV[2], m_backUV[3],
			0xFFFFFFFF, m_resource->getImage(0) );

		for( int i = 1; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			renderEngine->renderImage( &(m_quads[i].a), mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ), 0xFFFFFFFF
				, m_resource->getImage(i) );
		}
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

		m_quads[1].a = wm.v2.v2;
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

		float s = m_clientSize.x / m_initialSizes[0].x;
		float t = m_clientSize.y / m_initialSizes[0].y;
		m_backUV[0] = mt::vec2f::zero_v2;
		m_backUV[1] = mt::vec2f( s, 0.0f );
		m_backUV[2] = mt::vec2f( s, t );
		m_backUV[3] = mt::vec2f( 0.0f, t );

		m_invalidateQuads = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Window::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );
		mt::add_internal_point( _boundingBox, m_quads[1].a );
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
	void Window::setClientSize( const mt::vec2f& _clientSize )
	{
		m_clientSize = _clientSize;
		invalidateQuads();
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge