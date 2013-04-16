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
		: m_resourceHIT(NULL)
        , m_alphaTest(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpotImage::~HotSpotImage()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::_compile()
	{
		if( HotSpot::_compile() == false )
		{
			return false;
		}

        if( m_resourceHITName.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotSpotImage::_compile: '%s' can't set HIT resource"
                , this->getName().c_str()
                );

            return false;
        }

        m_resourceHIT = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceT<ResourceHIT>( m_resourceHITName );

        if( m_resourceHIT == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotSpotImage::_compile: '%s' can't get HIT resource '%s'"
                , this->getName().c_str()
                , m_resourceHITName.c_str()
                );

            return false;
        }
        
		float hs_width = (float)m_resourceHIT->getWidth();
        float hs_height = (float)m_resourceHIT->getHeight();

		Polygon polygon;

		boost::geometry::append(polygon, mt::vec2f(0.f, 0.f) );
		boost::geometry::append(polygon, mt::vec2f(hs_width, 0.f) );
		boost::geometry::append(polygon, mt::vec2f(hs_width, hs_height) );
		boost::geometry::append(polygon, mt::vec2f(0.f, hs_height) );

		this->setPolygon( polygon );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpotImage::_release()
	{
		if( m_resourceHIT != 0 )
		{
			m_resourceHIT->decrementReference();
			m_resourceHIT = 0;
		}

		HotSpot::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testPolygon( const mt::mat4f& _transform, const Polygon& _screenPoly, const mt::mat4f& _screenTransform )
	{
		if( this->isActivate() == false )
		{
			return false;
		}

		if( HotSpot::testPolygon( _transform, _screenPoly, _screenTransform ) == false )
		{
			return false;
		}

		const Polygon::ring_type & ring = _screenPoly.outer();

		if( ring.size() != 1 )
		{
			return true;
		}

		const mt::vec2f & ring_point = ring[0];

		mt::vec2f point;
		mt::mul_v2_m4( point, ring_point, _screenTransform );

		mt::mat4f invWM;
		mt::inv_m4( invWM, _transform );

		mt::vec2f pointIn;
		mt::mul_v2_m4( pointIn, point, invWM );

		bool result = m_resourceHIT->testPoint( pointIn, m_alphaTest );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testRadius( const mt::mat4f& _transform, float _radius, const mt::mat4f& _screenTransform )
	{
		if( this->isActivate() == false )
		{
			return false;
		}
		
		mt::vec2f point;
		mt::mul_v2_m4( point, mt::vec2f(0.f, 0.f), _screenTransform );

		mt::mat4f invWM;
		mt::inv_m4( invWM, _transform );

		mt::vec2f pointIn;
		mt::mul_v2_m4( pointIn, point, invWM );

		bool result = m_resourceHIT->testRadius( pointIn, _radius, m_alphaTest );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceHIT * HotSpotImage::getResourseHIT() const
	{
		return m_resourceHIT;
	}
    //////////////////////////////////////////////////////////////////////////
    void HotSpotImage::setResourceHIT( const ConstString& _resourceName )
    {
        m_resourceHITName = _resourceName;

        this->recompile();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & HotSpotImage::getResourceHIT() const
    {
        return m_resourceHITName;
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
    size_t HotSpotImage::getWidth() const
    {
        if( m_resourceHIT == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("HotSpot::getWidth %s not compiled"
                , this->getName().c_str()
                );

            return 0;
        }

        size_t width = m_resourceHIT->getWidth();

        return width;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t HotSpotImage::getHeight() const
    {
        if( m_resourceHIT == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("HotSpot::getHeight %s not compiled"
                , this->getName().c_str()
                );

            return 0;
        }

        size_t height = m_resourceHIT->getHeight();

        return height;
    }
}	// namespace Menge
