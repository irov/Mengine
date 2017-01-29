#	include "HotSpotImage.h"

#	include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Kernel/RenderCameraHelper.h"

#   include "ResourceHIT.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Consts.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotSpotImage::HotSpotImage()
		: m_alphaTest(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpotImage::~HotSpotImage()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::setResourceHIT( const ResourceHITPtr & _resourceHIT )
	{
		if( m_resourceHIT == _resourceHIT )
		{
			return;
		}

		m_resourceHIT = _resourceHIT;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceHITPtr & HotSpotImage::getResourceHIT() const
	{
		return m_resourceHIT;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::setAlphaTest( float _value )
	{
		m_alphaTest = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	float HotSpotImage::getAlphaTest() const
	{
		return m_alphaTest;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t HotSpotImage::getWidth() const
	{
		if( m_resourceHIT == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::getWidth %s not compiled"
				, this->getName().c_str()
				);

			return 0;
		}

		uint32_t width = m_resourceHIT->getWidth();

		return width;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t HotSpotImage::getHeight() const
	{
		if( m_resourceHIT == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::getHeight %s not compiled"
				, this->getName().c_str()
				);

			return 0;
		}

		uint32_t height = m_resourceHIT->getHeight();

		return height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::_compile()
	{
		if( HotSpot::_compile() == false )
		{
			return false;
		}

        if( m_resourceHIT == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("HotSpotImage::_compile: '%s' resource is null"
                , this->getName().c_str()
                );

            return false;
        }

        if( m_resourceHIT.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("HotSpotImage::_compile: '%s' can't compile HIT resource '%s'"
                , this->getName().c_str()
                , m_resourceHIT->getName().c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::_release()
	{
        m_resourceHIT.release();

		HotSpot::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testPoint( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const
	{
		(void)_contentResolution;
		(void)_viewport;

		if( m_global == true )
		{
			return !m_outward;
		}

		const mt::box2f & bb = this->getBoundingBox();

		mt::box2f bb_screen;
        Helper::worldToScreenBox( _camera, _viewport, _contentResolution, bb, bb_screen );

		if( mt::is_intersect( bb_screen, _point ) == false )
		{
			return m_outward;
		}

        const mt::mat4f & vpm = _camera->getCameraViewProjectionMatrix();
        const Viewport & vp = _viewport->getViewport();

		mt::mat4f vpm_inv;
		mt::inv_m4_m4( vpm_inv, vpm );

		mt::vec2f contentResolutionSize;
		_contentResolution.calcSize( contentResolutionSize );

		mt::vec2f point_vp;
		point_vp = _point * contentResolutionSize;

		point_vp -= vp.begin;
		point_vp /= vp.size();

		mt::vec2f point_norm;
		point_norm.x = point_vp.x * 2.f - 1.f;
		point_norm.y = 1.f - point_vp.y * 2.f;

		mt::vec2f pointIn1;
		mt::mul_v2_v2_m4( pointIn1, point_norm, vpm_inv );

		const mt::mat4f & wm = this->getWorldMatrix();

		mt::mat4f invWM;
		mt::inv_m4_m4( invWM, wm );

		mt::vec2f pointIn2;
		mt::mul_v2_v2_m4( pointIn2, pointIn1, invWM );

		bool result = m_resourceHIT->testPoint( pointIn2, m_alphaTest );

		return result != m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testRadius( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radius ) const
	{
		(void)_contentResolution;
		(void)_viewport;
		
		if( m_global == true )
		{
			return !m_outward;
		}

        const mt::box2f & bb = this->getBoundingBox();

        mt::box2f bb_screen;
        Helper::worldToScreenBox( _camera, _viewport, _contentResolution, bb, bb_screen );

		if( mt::is_intersect( bb_screen, _point, _radius ) == false )
		{
			return m_outward;
		}

        const mt::mat4f & vpm = _camera->getCameraViewProjectionMatrix();
        const Viewport & vp = _viewport->getViewport();

        mt::mat4f vpm_inv;
        mt::inv_m4_m4( vpm_inv, vpm );

        mt::vec2f contentResolutionSize;
        _contentResolution.calcSize( contentResolutionSize );

        mt::vec2f point_vp;
        point_vp = _point * contentResolutionSize;

        point_vp -= vp.begin;
        point_vp /= vp.size();

        mt::vec2f point_norm;
        point_norm.x = point_vp.x * 2.f - 1.f;
        point_norm.y = 1.f - point_vp.y * 2.f;

        mt::vec2f pointIn1;
        mt::mul_v2_v2_m4( pointIn1, point_norm, vpm_inv );

        const mt::mat4f & wm = this->getWorldMatrix();

        mt::mat4f invWM;
        mt::inv_m4_m4( invWM, wm );

        mt::vec2f pointIn2;
        mt::mul_v2_v2_m4( pointIn2, pointIn1, invWM );
		
		bool result = m_resourceHIT->testRadius( pointIn2, _radius, m_alphaTest );

		return result != m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testPolygon( const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
	{
		(void)_contentResolution;
		(void)_viewport;
		
		if( m_global == true )
		{
			return !m_outward;
		}

		if( _polygon.empty() == true )
		{
			return m_outward;
		}

        const mt::box2f & bb = this->getBoundingBox();

        mt::box2f bb_screen;
        Helper::worldToScreenBox( _camera, _viewport, _contentResolution, bb, bb_screen );

        mt::box2f bb_polygon;
        _polygon.to_box2f( bb_polygon );

        mt::box2f bb_polygon_screen;
        Helper::worldToScreenBox( _camera, _viewport, _contentResolution, bb_polygon, bb_polygon_screen );

        transpose_box( bb_polygon_screen, _point );

        if( mt::is_intersect( bb_screen, bb_polygon_screen ) == false )
        {
            return m_outward;
        }

		return !m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		const mt::mat4f & wm = this->getWorldMatrix();
		
		float hs_width = (float)m_resourceHIT->getWidth();
		float hs_height = (float)m_resourceHIT->getHeight();

		mt::vec2f minimal(0.f, 0.f);
		mt::vec2f maximal(hs_width, hs_height);

		mt::vec2f minimal_wm;
		mt::mul_v2_v2_m4( minimal_wm, minimal, wm );

		mt::vec2f maximal_wm;
		mt::mul_v2_v2_m4( maximal_wm, maximal, wm );

		mt::set_box_from_two_point( _boundingBox, minimal_wm, maximal_wm );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::_debugRender( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, unsigned int _debugMask )
	{
		if( (_debugMask & MENGE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		if( m_debugColor == 0x00000000 )
		{
			return;
		}

		const mt::box2f & box = this->getBoundingBox();

		RenderVertex2D * vertices = _renderService
			->getDebugRenderVertex2D( 4 * 2 );

		if( vertices == nullptr )
		{
			return;
		}

		vertices[0].position.x = box.minimum.x;
		vertices[0].position.y = box.minimum.y;

		vertices[1].position.x = box.maximum.x;
		vertices[1].position.y = box.minimum.y;

		vertices[2].position.x = box.maximum.x;
		vertices[2].position.y = box.minimum.y;

		vertices[3].position.x = box.maximum.x;
		vertices[3].position.y = box.maximum.y;

		vertices[4].position.x = box.maximum.x;
		vertices[4].position.y = box.maximum.y;

		vertices[5].position.x = box.minimum.x;
		vertices[5].position.y = box.maximum.y;

		vertices[6].position.x = box.minimum.x;
		vertices[6].position.y = box.maximum.y;

		vertices[7].position.x = box.minimum.x;
		vertices[7].position.y = box.minimum.y;


		for( uint32_t i = 0; i != 8; ++i )
		{
			vertices[i].position.z = 0.f;

			vertices[i].color = m_debugColor;
			vertices[i].uv[0].x = 0.f;
			vertices[i].uv[0].y = 0.f;
			vertices[i].uv[1].x = 0.f;
			vertices[i].uv[1].y = 0.f;
		}

		const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getDebugMaterial();

		_renderService
			->addRenderLine( _state, debugMaterial, vertices, 8, nullptr, true );
	}
}	// namespace Menge
