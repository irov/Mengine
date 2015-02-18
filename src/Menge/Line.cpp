#	include "Line.h"

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/StringizeInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Line::Line()
		: m_from(0.f, 0.f, 0.f)
		, m_to(0.f, 0.f, 0.f)
		, m_width(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Line::setFrom( const mt::vec3f & _value )
	{
		m_from = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Line::getFrom() const
	{
		return m_from;
	}
	//////////////////////////////////////////////////////////////////////////
	void Line::setTo( const mt::vec3f & _value )
	{
		m_to = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & Line::getTo() const
	{
		return m_to;
	}
	//////////////////////////////////////////////////////////////////////////
	void Line::setWidth( float _width )
	{
		m_width = _width;
	}
	//////////////////////////////////////////////////////////////////////////
	float Line::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	void Line::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		Node::_render( _viewport, _camera );

		const mt::mat4f & wm = this->getWorldMatrix();

		mt::vec3f fromWM;
		mt::mul_m4_v3( fromWM, wm, m_from );

		mt::vec3f toWM;
		mt::mul_m4_v3( toWM, wm, m_to );

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
			m_vertices[i].uv.x = 0.f;
			m_vertices[i].uv.y = 0.f;
			m_vertices[i].uv2.x = 0.f;
			m_vertices[i].uv2.y = 0.f;
		}

		RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getMaterial( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Debug"), false, false, PT_TRIANGLELIST, 0, nullptr );

		RENDER_SERVICE(m_serviceProvider)
			->addRenderQuad( _viewport, _camera, material, m_vertices, 4, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////

}
