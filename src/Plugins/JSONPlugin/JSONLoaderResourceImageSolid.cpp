#include "JSONLoaderResourceImageSolid.h"

#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageSolid::JSONLoaderResourceImageSolid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageSolid::~JSONLoaderResourceImageSolid()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceImageSolid::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceImageSolid * resource = _factorable.getT<ResourceImageSolid *>();

        Color color;
        if( Helper::getJSONMemberColor( _json, "Color", "Value", &color ) == false )
        {
            return false;
        }

        resource->setColor( color );

        mt::vec2f size;
        if( Helper::getJSONMemberVec2f( _json, "Size", "Value", &size ) == false )
        {
            return false;
        }

        resource->setMaxSize( size );
        resource->setSize( size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
