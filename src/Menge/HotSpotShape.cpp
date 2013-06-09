#	include "HotSpotShape.h"

#	include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "ResourceShape.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Consts.h"


namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotShape::HotSpotShape()
        : m_resourceShape(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotShape::~HotSpotShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotShape::_compile()
    {
        if( HotSpot::_compile() == false )
        {
            return false;
        }

        if( m_resourceShapeName.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotSpotShape::_compile: '%s' can't set Shape resource"
                , this->getName().c_str()
                );

            return false;
        }

        m_resourceShape = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceT<ResourceShape>( m_resourceShapeName );

        if( m_resourceShape == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotSpotShape::_compile: '%s' can't get HIT resource '%s'"
                , this->getName().c_str()
                , m_resourceShapeName.c_str()
                );

            return false;
        }

        const Polygon & polygon = m_resourceShape->getPolygon();

        this->setPolygon( polygon );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotShape::_release()
    {
        if( m_resourceShape != nullptr )
        {
            m_resourceShape->decrementReference();
            m_resourceShape = nullptr;
        }

        HotSpot::_release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotShape::testPolygon( const mt::mat4f& _transform, const Polygon& _screenPoly, const mt::mat4f& _screenTransform )
    {
        if( this->isActivate() == false )
        {
            return false;
        }

        if( HotSpot::testPolygon( _transform, _screenPoly, _screenTransform ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotShape::testRadius( const mt::mat4f& _transform, float _radius, const mt::mat4f& _screenTransform )
    {
        if( this->isActivate() == false )
        {
            return false;
        }

        if( HotSpot::testRadius( _transform, _radius, _screenTransform ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceShape * HotSpotShape::getResourseShape() const
    {
        return m_resourceShape;
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotShape::setResourceShapeName( const ConstString& _resourceName )
    {
        m_resourceShapeName = _resourceName;

        this->recompile();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & HotSpotShape::getResourceShapeName() const
    {
        return m_resourceShapeName;
    }
}	// namespace Menge
