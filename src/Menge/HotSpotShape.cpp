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

        if( m_resourceShape == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotSpotShape::_compile: '%s' resource is null"
                , this->getName().c_str()
                );

            return false;
        }

        if( m_resourceShape.compile() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "HotSpotShape::_compile: '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceShape->getName().c_str()
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
        m_resourceShape.release();

        HotSpot::_release();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotShape::setResourceShape( ResourceShape * _resourceShape )
    {
        if( m_resourceShape == _resourceShape )
        {
            return;
        }

        m_resourceShape = _resourceShape;

        this->recompile();
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceShape * HotSpotShape::getResourceShape() const
    {
        return m_resourceShape;
    }
}	// namespace Menge
