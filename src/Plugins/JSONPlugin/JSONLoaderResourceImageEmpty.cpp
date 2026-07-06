#include "JSONLoaderResourceImageEmpty.h"

#include "JSONLoaderResourceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageEmpty::JSONLoaderResourceImageEmpty()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceImageEmpty::~JSONLoaderResourceImageEmpty()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceImageEmpty::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceImageEmpty * resource = _factorable.getT<ResourceImageEmpty *>();

        mt::vec2f maxSize;
        if( Detail::getJSONMemberVec2Required( _json, "File", "MaxSize", &maxSize ) == false )
        {
            return false;
        }

        resource->setMaxSize( maxSize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
