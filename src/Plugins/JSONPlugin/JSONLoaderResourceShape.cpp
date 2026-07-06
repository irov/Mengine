#include "JSONLoaderResourceShape.h"

#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceShape::JSONLoaderResourceShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceShape::~JSONLoaderResourceShape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceShape::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceShape * resource = _factorable.getT<ResourceShape *>();

        Polygon polygon;
        if( Helper::getJSONMemberPolygon( _json, "Polygon", "Value", &polygon ) == false )
        {
            return false;
        }

        resource->setPolygon( polygon );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
