#   include "ResourceShape.h"
#   include "Kernel/ResourceImplement.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    RESOURCE_IMPLEMENT( ResourceShape );
    //////////////////////////////////////////////////////////////////////////
    ResourceShape::ResourceShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const Polygon & ResourceShape::getPolygon() const
    {
        return m_polygon;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceShape::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_DataBlock::Meta_ResourceShape * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceShape *>(_meta);

        metadata->swap_Polygon_Value( m_polygon );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceShape::_compile()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceShape::_release()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceShape::_isValid() const
    {
        return true;
    }
}