#include "MetabufLoaderResourceShape.h"

#include "Engine/ResourceShape.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceShape::MetabufLoaderResourceShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceShape::~MetabufLoaderResourceShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceShape::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceShape * resource = _factorable.getT<ResourceShape *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceShape * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceShape *>(_meta);

        const Polygon & polygon = metadata->get_Polygon_Value();
        resource->setPolygon( polygon );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}