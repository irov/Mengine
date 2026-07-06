#include "JSONLoaderResourceHIT.h"

#include "JSONLoaderResourceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceHIT::JSONLoaderResourceHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceHIT::~JSONLoaderResourceHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceHIT::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceHIT * resource = _factorable.getT<ResourceHIT *>();

        const ContentInterfacePtr & content = resource->getContent();

        Detail::setJSONContentFile( content, _json, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
