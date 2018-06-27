#include "HotSpotCircle.h"

#include "Interface/RenderCameraInterface.h"

#include "Logger/Logger.h"

#include "math/angle.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	HotSpotCircle::HotSpotCircle()
		: m_enumerator(0)
		, m_radius(0.f)
		, m_ellipse(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpotCircle::~HotSpotCircle()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotCircle::setRadius( float _radius )
	{		
		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float HotSpotCircle::getRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotCircle::setEllipse( float _ellipse )
	{
		m_ellipse = _ellipse;
	}
	//////////////////////////////////////////////////////////////////////////
	float HotSpotCircle::getEllipse() const
	{
		return m_ellipse;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotCircle::testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const
	{
		(void)_contentResolution;
		(void)_viewport;
		
		if( m_global == true )
		{
			return !m_outward;
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		const mt::mat4f & vpm = _camera->getCameraViewProjectionMatrix();

		mt::mat4f wvpm;
		mt::mul_m4_m4( wvpm, wm, vpm );

		mt::vec2f v_wvp;
		mt::mul_v2_v2_m4_homogenize( v_wvp, mt::vec2f(0.f, 0.f), wvpm );

		mt::vec2f v_wvpn;
		v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
		v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

		mt::vec2f contentResolutionSize;
		_contentResolution.calcSize( contentResolutionSize );

		mt::vec2f v = (_point - v_wvpn) * contentResolutionSize;

		v.y /= m_ellipse;

		float v_sqrlength = v.sqrlength();

		if( v_sqrlength < m_radius * m_radius )
		{
			return !m_outward;
		}

		return m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotCircle::testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const
	{
		(void)_contentResolution;
		(void)_viewport;
		
		if( m_global == true )
		{
			return !m_outward;
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		const mt::mat4f & vpm = _camera->getCameraViewProjectionMatrix();

		mt::mat4f wvpm;
		mt::mul_m4_m4( wvpm, wm, vpm );

		mt::vec2f v_wvp;
		mt::mul_v2_v2_m4_homogenize( v_wvp, mt::vec2f( 0.f, 0.f ), wvpm );

		mt::vec2f v_wvpn;
		v_wvpn.x = (1.f + v_wvp.x) * 0.5f;
		v_wvpn.y = (1.f - v_wvp.y) * 0.5f;

		mt::vec2f contentResolutionSize;
		_contentResolution.calcSize( contentResolutionSize );

		mt::vec2f v = (_point - v_wvpn) * contentResolutionSize;

		v.y /= m_ellipse;

		float v_sqrlength = v.sqrlength();

		if( v_sqrlength < (m_radius + _radius) * (m_radius + _radius) )
		{
			return !m_outward;
		}

		return m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotCircle::testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
	{
		if( m_global == true )
		{
			return !m_outward;
		}

		//TODO:
		(void)_polygon;

		bool test = this->testPoint( _camera, _viewport, _contentResolution, _point );

		return test;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotCircle::_debugRender( const RenderContext * _state )
	{
		if( (_state->debugMask & MENGINE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		if( m_debugColor == 0x00000000 )
		{
			return;
		}

		uint32_t numpoints = 16;

        uint32_t vertexCount = numpoints * 2;

        RenderVertex2D * vertices = this->getDebugRenderVertex2D( vertexCount );

		if( vertices == nullptr )
		{
			return;
		}

		const mt::mat4f & wm = this->getWorldMatrix();
		
		for( uint32_t i = 0; i != numpoints; ++i )
		{
            uint32_t j = (i + 1) % numpoints;

			float x0 = ::cosf( mt::constant::two_pi / numpoints * i ) * m_radius;
			float x1 = ::cosf( mt::constant::two_pi / numpoints * j ) * m_radius;
			float y0 = ::sinf( mt::constant::two_pi / numpoints * i ) * m_radius;
			float y1 = ::sinf( mt::constant::two_pi / numpoints * j ) * m_radius;

			mt::vec3f trP0;
			mt::mul_v3_v2_m4( trP0, mt::vec2f( x0, y0 ), wm );

			RenderVertex2D & v0 = vertices[i * 2 + 0];

			v0.position = trP0;

			v0.color = m_debugColor;

			for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
			{
				v0.uv[uv_index].x = 0.f;
				v0.uv[uv_index].y = 0.f;
			}

			mt::vec3f trP1;
			mt::mul_v3_v2_m4( trP1, mt::vec2f( x1, y1 ), wm );

			RenderVertex2D & v1 = vertices[i * 2 + 1];

			v1.position = trP1;

			v1.color = m_debugColor;

			for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
			{
				v1.uv[uv_index].x = 0.f;
				v1.uv[uv_index].y = 0.f;
			}
		}

        const RenderMaterialInterfacePtr & debugMaterial = this->getDebugMaterial();

        this->addRenderLine( _state, debugMaterial
			, vertices
			, (uint32_t)vertexCount
			, nullptr
			, true
			);
	}
}	
