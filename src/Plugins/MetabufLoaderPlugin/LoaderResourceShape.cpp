#include "LoaderResourceShape.h"

#include "Engine/ResourceShape.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceShape::LoaderResourceShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceShape::~LoaderResourceShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceShape::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceShape * resource = _loadable.getT<ResourceShape *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceShape * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceShape *>(_meta);

        const Polygon & polygon = metadata->get_Polygon_Value();
        resource->setPolygon( polygon );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}