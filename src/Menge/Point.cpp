#	include "Point.h"

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/StringizeInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Point::Point()
		: m_linked(nullptr)
		, m_owner(false)
		, m_width(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::setLinkedPoint( Point * _linked )
	{
		if( m_linked != nullptr )
		{
			return;
		}

		m_linked = _linked;
		m_owner = true;

		_linked->m_linked = this;
		_linked->m_owner = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::removeLinkedPoint()
	{
		if( m_linked == nullptr )
		{
			return;
		}

		m_linked->m_linked = nullptr;
		m_linked->m_owner = false;

		m_linked = nullptr;
		m_owner = false;
	}
	//////////////////////////////////////////////////////////////////////////
	Point * Point::getLinkedPoint() const
	{
		return m_linked;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::setWidth( float _width )
	{
		m_width = _width;
	}
	//////////////////////////////////////////////////////////////////////////
	float Point::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_destroy()
	{
		this->removeLinkedPoint();

		Node::_destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_render( const RenderObjectState * _state )
	{
		Node::_render( _state );

		if( m_linked == nullptr )
		{
			return;
		}

		if( m_owner == false )
		{
			return;
		}

		const mt::mat4f & wm1 = this->getWorldMatrix();
		const mt::mat4f & wm2 = m_linked->getWorldMatrix();

		mt::vec3f fromWM;
		mt::mul_v3_m4( fromWM, mt::vec3f(0.f, 0.f, 0.f), wm1 );

		mt::vec3f toWM;
		mt::mul_v3_m4( toWM, mt::vec3f(0.f, 0.f, 0.f), wm2 );

		mt::vec3f dir;
		mt::sub_v3_v3( dir, toWM, fromWM );

		mt::vec3f dir_norm;
		mt::norm_v3( dir_norm, dir );

		mt::vec2f perp;
		mt::perp_v2( perp, mt::vec2f(dir_norm.x, dir_norm.y) );

		m_vertices[0].pos.x = fromWM.x + perp.x * m_width;
		m_vertices[0].pos.y = fromWM.y + perp.y * m_width;
		m_vertices[0].pos.z = fromWM.z;

		m_vertices[1].pos.x = toWM.x + perp.x * m_width;
		m_vertices[1].pos.y = toWM.y + perp.y * m_width;
		m_vertices[1].pos.z = toWM.z;

		m_vertices[2].pos.x = toWM.x - perp.x * m_width;
		m_vertices[2].pos.y = toWM.y - perp.y * m_width;
		m_vertices[2].pos.z = toWM.z;

		m_vertices[3].pos.x = fromWM.x - perp.x * m_width;
		m_vertices[3].pos.y = fromWM.y - perp.y * m_width;
		m_vertices[3].pos.z = fromWM.z;

		ColourValue color;
		this->calcTotalColor(color);

		uint32_t argb = color.getAsARGB();

		for( uint32_t i = 0; i != 4; ++i )
		{
			m_vertices[i].color = argb;
			m_vertices[i].uv[0].x = 0.f;
			m_vertices[i].uv[0].y = 0.f;
			m_vertices[i].uv[1].x = 0.f;
			m_vertices[i].uv[1].y = 0.f;
		}

		RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getMaterial( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Debug" ), PT_TRIANGLELIST, 0, nullptr );

		RENDER_SERVICE(m_serviceProvider)
			->addRenderQuad( _state, material, m_vertices, 4, nullptr, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_debugRender( const RenderObjectState * _state, unsigned int _debugMask )
	{
		if( (_debugMask & MENGE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		const mt::vec3f & pos = this->getWorldPosition();

		RenderVertex2D * vertices = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( 4 * 2 );

		if( vertices == nullptr )
		{
			return;
		}

		float debugWidth = 5.f;

		vertices[0].pos.x = pos.x - debugWidth;
		vertices[0].pos.y = pos.y - debugWidth;
		vertices[0].pos.z = pos.z;

		vertices[1].pos.x = pos.x + debugWidth;
		vertices[1].pos.y = pos.y - debugWidth;
		vertices[1].pos.z = pos.z;

		vertices[2].pos.x = pos.x + debugWidth;
		vertices[2].pos.y = pos.y - debugWidth;
		vertices[2].pos.z = pos.z;

		vertices[3].pos.x = pos.x + debugWidth;
		vertices[3].pos.y = pos.y + debugWidth;
		vertices[3].pos.z = pos.z;

		vertices[4].pos.x = pos.x + debugWidth;
		vertices[4].pos.y = pos.y + debugWidth;
		vertices[4].pos.z = pos.z;

		vertices[5].pos.x = pos.x - debugWidth;
		vertices[5].pos.y = pos.y + debugWidth;
		vertices[5].pos.z = pos.z;

		vertices[6].pos.x = pos.x - debugWidth;
		vertices[6].pos.y = pos.y + debugWidth;
		vertices[6].pos.z = pos.z;

		vertices[7].pos.x = pos.x - debugWidth;
		vertices[7].pos.y = pos.y - debugWidth;
		vertices[7].pos.z = pos.z;


		for( uint32_t i = 0; i != 8; ++i )
		{
			vertices[i].color = 0xFF00FF00;
			vertices[i].uv[0].x = 0.f;
			vertices[i].uv[0].y = 0.f;
			vertices[i].uv[1].x = 0.f;
			vertices[i].uv[1].y = 0.f;
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getDebugMaterial();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderLine( _state, debugMaterial, vertices, 8, nullptr, true );
	}
	//////////////////////////////////////////////////////////////////////////

}
