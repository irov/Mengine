#	include "HotSpotImage.h"

#	include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"

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
	void HotSpotImage::setResourceHIT( ResourceHIT * _resourceHIT )
	{
		if( m_resourceHIT == _resourceHIT )
		{
			return;
		}

		m_resourceHIT = _resourceHIT;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceHIT * HotSpotImage::getResourceHIT() const
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
	bool HotSpotImage::testPoint( const mt::vec2f & _point ) const
	{
		const mt::box2f & bb = this->getBoundingBox();

		if( mt::is_intersect( bb, _point ) == false )
		{
			return m_outward;
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		mt::mat4f invWM;
		mt::inv_m4( invWM, wm );

		mt::vec2f pointIn1;
		mt::mul_v2_m4( pointIn1, _point, invWM );

		bool result = m_resourceHIT->testPoint( pointIn1, m_alphaTest );

		return result != m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testRadius( const mt::vec2f & _point, float _radius ) const
	{
		const mt::box2f & bb = this->getBoundingBox();

		if( mt::is_intersect( bb, _point, _radius ) == false )
		{
			return m_outward;
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		mt::mat4f invWM;
		mt::inv_m4( invWM, wm );

		mt::vec2f pointIn1;
		mt::mul_v2_m4( pointIn1, _point, invWM );
		
		bool result = m_resourceHIT->testRadius( pointIn1, _radius, m_alphaTest );

		return result != m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testPolygon( const mt::vec2f & _point, const Polygon & _polygon ) const
	{
		if( polygon_empty( _polygon ) == true )
		{
			return m_outward;
		}

		const mt::box2f & bb = this->getBoundingBox();

		mt::box2f bb_screen;
		if( polygon_to_box2f( bb_screen, _polygon ) == false )
		{
			return m_outward;
		}

		mt::transpose_box( bb_screen, _point );

		bool intersect = mt::is_intersect( bb, bb_screen );

		return intersect != m_outward;
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
		mt::mul_v2_m4( minimal_wm, minimal, wm );

		mt::vec2f maximal_wm;
		mt::mul_v2_m4( maximal_wm, maximal, wm );

		mt::set_box_from_min_max( _boundingBox, minimal_wm, maximal_wm );
	}
}	// namespace Menge
