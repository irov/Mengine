#include "Point.h"

#include "Interface/StringizeInterface.h"

namespace Mengine
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
    RenderMaterialInterfacePtr Point::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->getMaterial3( EM_DEBUG, PT_TRIANGLELIST, 0, nullptr );

        return material;
    }
	//////////////////////////////////////////////////////////////////////////
	void Point::_render( const RenderContext * _state )
	{
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
		mt::mul_v3_v3_m4( fromWM, mt::vec3f( 0.f, 0.f, 0.f ), wm1 );

		mt::vec3f toWM;
		mt::mul_v3_v3_m4( toWM, mt::vec3f( 0.f, 0.f, 0.f ), wm2 );

		mt::vec3f dir;
		mt::sub_v3_v3( dir, toWM, fromWM );

		mt::vec3f dir_norm;
		mt::norm_v3_v3( dir_norm, dir );

		mt::vec2f perp;
		mt::perp_v2( perp, mt::vec2f(dir_norm.x, dir_norm.y) );

		m_vertices[0].position.x = fromWM.x + perp.x * m_width;
		m_vertices[0].position.y = fromWM.y + perp.y * m_width;
		m_vertices[0].position.z = fromWM.z;

		m_vertices[1].position.x = toWM.x + perp.x * m_width;
		m_vertices[1].position.y = toWM.y + perp.y * m_width;
		m_vertices[1].position.z = toWM.z;

		m_vertices[2].position.x = toWM.x - perp.x * m_width;
		m_vertices[2].position.y = toWM.y - perp.y * m_width;
		m_vertices[2].position.z = toWM.z;

		m_vertices[3].position.x = fromWM.x - perp.x * m_width;
		m_vertices[3].position.y = fromWM.y - perp.y * m_width;
		m_vertices[3].position.z = fromWM.z;

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

        const RenderMaterialInterfacePtr & material = this->getMaterial();

		this->addRenderQuad( _state, material, m_vertices, 4, nullptr, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_debugRender( const RenderContext * _state )
	{
        if( (_state->debugMask & MENGINE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		const mt::vec3f & pos = this->getWorldPosition();

        const uint32_t vertexCount = 8;

        RenderVertex2D * vertices = this->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		float debugWidth = 5.f;

		vertices[0].position.x = pos.x - debugWidth;
		vertices[0].position.y = pos.y - debugWidth;
		vertices[0].position.z = pos.z;

		vertices[1].position.x = pos.x + debugWidth;
		vertices[1].position.y = pos.y - debugWidth;
		vertices[1].position.z = pos.z;

		vertices[2].position.x = pos.x + debugWidth;
		vertices[2].position.y = pos.y - debugWidth;
		vertices[2].position.z = pos.z;

		vertices[3].position.x = pos.x + debugWidth;
		vertices[3].position.y = pos.y + debugWidth;
		vertices[3].position.z = pos.z;

		vertices[4].position.x = pos.x + debugWidth;
		vertices[4].position.y = pos.y + debugWidth;
		vertices[4].position.z = pos.z;

		vertices[5].position.x = pos.x - debugWidth;
		vertices[5].position.y = pos.y + debugWidth;
		vertices[5].position.z = pos.z;

		vertices[6].position.x = pos.x - debugWidth;
		vertices[6].position.y = pos.y + debugWidth;
		vertices[6].position.z = pos.z;

		vertices[7].position.x = pos.x - debugWidth;
		vertices[7].position.y = pos.y - debugWidth;
		vertices[7].position.z = pos.z;


		for( uint32_t i = 0; i != vertexCount; ++i )
		{
			vertices[i].color = 0xFF00FF00;
			vertices[i].uv[0].x = 0.f;
			vertices[i].uv[0].y = 0.f;
			vertices[i].uv[1].x = 0.f;
			vertices[i].uv[1].y = 0.f;
		}

        const RenderMaterialInterfacePtr & material = this->getMaterial();

        this->addRenderLine( _state, material, vertices, vertexCount, nullptr, true );
	}
	//////////////////////////////////////////////////////////////////////////

}
