#include "HotSpotShape.h"

#include "Interface/ResourceServiceInterface.h"

#include "ResourceShape.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
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
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceShape, false, "'%s' resource is null"
            , this->getName().c_str()
        );

        if( m_resourceShape->compile() == false )
        {
            LOGGER_ERROR( "'%s' resource '%s' not compile"
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
        m_resourceShape->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotShape::setResourceShape( const ResourceShapePtr & _resourceShape )
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
    const ResourceShapePtr & HotSpotShape::getResourceShape() const
    {
        return m_resourceShape;
    }
}
