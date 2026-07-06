#include "JSONLoaderResourceJSON.h"

#include "JSONLoaderResourceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceJSON::JSONLoaderResourceJSON()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceJSON::~JSONLoaderResourceJSON()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceJSON::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceJSON * resource = _factorable.getT<ResourceJSON *>();

        const ContentInterfacePtr & content = resource->getContent();

        Detail::setJSONContentFile( content, _json, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
