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
	bool HotSpotImage::_compile()
	{
		if( HotSpot::_compile() == false )
		{
			return false;
		}

        if( m_resourceHIT == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotSpotImage::_compile: '%s' resource is null"
                , this->getName().c_str()
                );

            return false;
        }

        if( m_resourceHIT.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotSpotImage::_compile: '%s' can't compile HIT resource '%s'"
                , this->getName().c_str()
                , m_resourceHIT->getName().c_str()
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
        m_resourceHIT.release();

		HotSpot::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotImage::testRadius( const mt::mat4f& _transform, const mt::vec2f & _point, float _radius )
	{
		const mt::mat4f & wm = this->getWorldMatrix();

		mt::mat4f invWM;
		mt::inv_m4( invWM, wm );

		mt::vec2f pointIn1;
		mt::mul_v2_m4( pointIn1, _point, invWM );

		mt::vec2f pointIn2;
		mt::mul_v2_m4( pointIn2, _point, _transform );

		bool result = m_resourceHIT->testRadius( pointIn2, _radius, m_alphaTest );

		return result;
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
    size_t HotSpotImage::getWidth() const
    {
        if( m_resourceHIT == nullptr )
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
        if( m_resourceHIT == nullptr )
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
