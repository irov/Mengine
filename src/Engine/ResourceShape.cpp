#   include "ResourceShape.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
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
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceShape * metadata 
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceShape *>(_meta);

        m_polygon = metadata->get_Polygon_Value();

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