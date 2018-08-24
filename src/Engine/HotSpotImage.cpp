#include "HotSpotImage.h"

#include "Interface/ResourceInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/RenderCameraHelper.h"

#include "ResourceHIT.h"

#include "Kernel/Logger.h"

#include "Kernel/String.h"

namespace Mengine
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
	void HotSpotImage::setResourceTestPick( const ResourceTestPickPtr & _resourceTestPick )
	{
		if( m_resourceTestPick == _resourceTestPick )
		{
			return;
		}

        this->recompile( [this, _resourceTestPick]() {m_resourceTestPick = _resourceTestPick; } );
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceTestPickPtr & HotSpotImage::getResourceTestPick() const
	{
		return m_resourceTestPick;
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
		if( m_resourceTestPick == nullptr )
		{
			LOGGER_ERROR("HotSpot::getWidth %s not compiled"
				, this->getName().c_str()
				);

			return 0;
		}

		uint32_t width = m_resourceTestPick->getImageWidth();

		return width;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t HotSpotImage::getHeight() const
	{
		if( m_resourceTestPick == nullptr )
		{
			LOGGER_ERROR("HotSpot::getHeight %s not compiled"
				, this->getName().c_str()
				);

			return 0;
		}

		uint32_t height = m_resourceTestPick->getImageHeight();

		return height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::_compile()
	{
		if( HotSpot::_compile() == false )
		{
			return false;
		}

        if( m_resourceTestPick == nullptr )
        {
            LOGGER_ERROR("HotSpotImage::_compile: '%s' resource is null"
                , this->getName().c_str()
                );

            return false;
        }

        if( m_resourceTestPick.compile() == false )
        {
            LOGGER_ERROR("HotSpotImage::_compile: '%s' can't compile HIT resource '%s'"
                , this->getName().c_str()
                , m_resourceTestPick->getName().c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::_release()
	{
        m_resourceTestPick.release();

		HotSpot::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testPoint( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point ) const
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

        const mt::mat4f & vpm_inv = _camera->getCameraViewProjectionMatrixInv();
        const Viewport & vp = _viewport->getViewport();

		//mt::mat4f vpm_inv;
		//mt::inv_m4_m4( vpm_inv, vpm );

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

		bool result = m_resourceTestPick->testPoint( pointIn2, m_alphaTest );

		return result != m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testRadius( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, float _radiusx, float _radiusy ) const
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

        if( mt::is_intersect( bb_screen, _point, _radiusx, _radiusy ) == false )
		{
			return m_outward;
		}

        const mt::mat4f & vpm_inv = _camera->getCameraViewProjectionMatrixInv();
        const Viewport & vp = _viewport->getViewport();

        //mt::mat4f vpm_inv;
        //mt::inv_m4_m4( vpm_inv, vpm );

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
		
		bool result = m_resourceTestPick->testRadius( pointIn2, _radiusx, m_alphaTest );

		return result != m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testPolygon( const RenderCameraInterfacePtr & _camera, const RenderViewportInterfacePtr & _viewport, const Resolution & _contentResolution, const mt::vec2f & _point, const Polygon & _polygon ) const
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
		
		float hs_width = (float)m_resourceTestPick->getImageWidth();
		float hs_height = (float)m_resourceTestPick->getImageHeight();

		mt::vec2f minimal(0.f, 0.f);
		mt::vec2f maximal(hs_width, hs_height);

		mt::vec2f minimal_wm;
		mt::mul_v2_v2_m4( minimal_wm, minimal, wm );

		mt::vec2f maximal_wm;
		mt::mul_v2_v2_m4( maximal_wm, maximal, wm );

		mt::set_box_from_two_point( _boundingBox, minimal_wm, maximal_wm );
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::_debugRender( const RenderContext * _state )
	{
		if( (_state->debugMask & MENGINE_DEBUG_HOTSPOTS) == 0 )
		{
			return;
		}

		if( m_debugColor == 0x00000000 )
		{
			return;
		}

		const mt::box2f & box = this->getBoundingBox();

        RenderVertex2D * vertices = this->getDebugRenderVertex2D( 4 * 2 );

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

        const RenderMaterialInterfacePtr & debugMaterial = this->getDebugMaterial();

        this->addRenderLine( _state, debugMaterial, vertices, 8, nullptr, true );
	}
}	
