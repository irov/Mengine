#include "HotSpotResourceShape.h"

#include "ResourceShape.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotResourceShape::HotSpotResourceShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotResourceShape::~HotSpotResourceShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool HotSpotResourceShape::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceShape, "'%s' resource is nullptr"
            , this->getName().c_str()
        );

        if( m_resourceShape->compile() == false )
        {
            LOGGER_ERROR( "hotspot resource shape '%s' resource '%s' not compile"
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
    void HotSpotResourceShape::_release()
    {
        m_resourceShape->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotResourceShape::_dispose()
    {
        m_resourceShape = nullptr;

        HotSpotPolygon::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotResourceShape::setResourceShape( const ResourceShapePtr & _resourceShape )
    {
        if( m_resourceShape == _resourceShape )
        {
            return;
        }

        this->recompile( [this, &_resourceShape]()
        {
            m_resourceShape = _resourceShape;

            if( m_resourceShape == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceShapePtr & HotSpotResourceShape::getResourceShape() const
    {
        return m_resourceShape;
    }
    //////////////////////////////////////////////////////////////////////////
}
