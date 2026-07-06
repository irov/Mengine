#include "JSONLoaderResourceCursorICO.h"

#include "JSONLoaderResourceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceCursorICO::JSONLoaderResourceCursorICO()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceCursorICO::~JSONLoaderResourceCursorICO()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceCursorICO::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceCursorICO * resource = _factorable.getT<ResourceCursorICO *>();

        const ContentInterfacePtr & content = resource->getContent();

        Detail::setJSONContentFile( content, _json, false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
