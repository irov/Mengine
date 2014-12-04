#	include "Line.h"

#   include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Line::Line()
		: m_from(0.f, 0.f, 0.f)
		, m_to(0.f, 0.f, 0.f)
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
	void Line::_debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		(void)_viewport;
        (void)_camera;
        (void)_debugMask;

		if( (_debugMask & MENGE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		mt::vec3f fromWM;
		mt::mul_m4_v3( fromWM, wm, m_from );

		mt::vec3f toWM;
		mt::mul_m4_v3( toWM, wm, m_to );

		RenderVertex2D * vertexLine = RENDER_SERVICE(m_serviceProvider)
			->getDebugRenderVertex2D( 1 * 2 );

		vertexLine[0].pos.x = fromWM.x;
		vertexLine[0].pos.y = fromWM.y;
		vertexLine[0].pos.z = fromWM.z;

		vertexLine[1].pos.x = toWM.x;
		vertexLine[1].pos.y = toWM.y;
		vertexLine[1].pos.z = toWM.z;

	
		for( uint32_t i = 0; i != 2; ++i )
		{
			vertexLine[i].color = 0xFF00FF00;
			vertexLine[i].uv.x = 0.f;
			vertexLine[i].uv.y = 0.f;
			vertexLine[i].uv2.x = 0.f;
			vertexLine[i].uv2.y = 0.f;
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDER_SERVICE(m_serviceProvider)
			->getDebugMaterial();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderLine( _viewport, _camera, debugMaterial, vertexLine, 2, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////

}
